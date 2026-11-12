#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
center_stage_origin.py
ステージ(部屋・通路)の glTF の原点を「床の中央(XZ)」に移動するスクリプト。

【やっていること】
  1. 床のノード(名前が Tile / Floor で始まるもの)の XZ の範囲を調べる
  2. その範囲の真ん中が (0, 0) に来るように、シーン直下のノードを平行移動する
  3. 新しい .gltf として保存する(頂点データが入った .bin は触らない)

【Y(高さ)は動かしません】
  床の上面が y=0 のまま残るので、「原点 = 床の中央の高さ0」になります。

【使い方】(Python 3 だけで動きます。追加インストール不要)
  python center_stage_origin.py Room_7x7\\Room_7x7.gltf
      → Room_7x7\\Room_7x7_Centered.gltf を作る(元ファイルはそのまま)

  python center_stage_origin.py .
      → フォルダ内の .gltf をまとめて処理(床が見つからないものは自動でスキップ)

  オプション
    --inplace     元の .gltf を上書きする(元は *.gltf.orig に退避)
    --suffix X    出力ファイル名の末尾を変える(既定: _Centered)
    --mode all    床ではなく「全ジオメトリ」のバウンディングボックス中心を使う
    --dry-run     ファイルは書かずに、移動量だけ表示する
"""

import argparse
import json
import math
import os
import shutil
import sys

EPS = 1e-4  # これ以下のズレは「すでに中央」とみなす


# ---------------------------------------------------------------- 行列まわり
# 4x4 行列は「列優先(glTF と同じ)」の長さ16のリストで持つ。

def identity():
    return [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]


def mat_mul(a, b):
    """a @ b (列優先)"""
    r = [0.0] * 16
    for col in range(4):
        for row in range(4):
            r[col * 4 + row] = sum(a[k * 4 + row] * b[col * 4 + k] for k in range(4))
    return r


def node_local_matrix(node):
    """ノードのローカル変換行列 (matrix か T*R*S から作る)"""
    if "matrix" in node:
        return list(node["matrix"])
    tx, ty, tz = node.get("translation", [0, 0, 0])
    x, y, z, w = node.get("rotation", [0, 0, 0, 1])
    sx, sy, sz = node.get("scale", [1, 1, 1])
    # クォータニオン → 回転行列
    r00 = 1 - 2 * (y * y + z * z)
    r01 = 2 * (x * y - z * w)
    r02 = 2 * (x * z + y * w)
    r10 = 2 * (x * y + z * w)
    r11 = 1 - 2 * (x * x + z * z)
    r12 = 2 * (y * z - x * w)
    r20 = 2 * (x * z - y * w)
    r21 = 2 * (y * z + x * w)
    r22 = 1 - 2 * (x * x + y * y)
    # T * R * S (列優先で並べる)
    return [
        r00 * sx, r10 * sx, r20 * sx, 0,
        r01 * sy, r11 * sy, r21 * sy, 0,
        r02 * sz, r12 * sz, r22 * sz, 0,
        tx, ty, tz, 1,
    ]


def transform_point(m, p):
    x, y, z = p
    return (
        m[0] * x + m[4] * y + m[8] * z + m[12],
        m[1] * x + m[5] * y + m[9] * z + m[13],
        m[2] * x + m[6] * y + m[10] * z + m[14],
    )


# ---------------------------------------------------------------- 範囲の計算

def collect_world_boxes(gltf):
    """
    メッシュを持つノードごとに、ワールド座標でのバウンディングボックスを集める。
    戻り値: [(ノード名, (minx,miny,minz), (maxx,maxy,maxz)), ...]
    """
    nodes = gltf["nodes"]
    boxes = []

    def visit(index, parent):
        node = nodes[index]
        world = mat_mul(parent, node_local_matrix(node))
        if "mesh" in node:
            for prim in gltf["meshes"][node["mesh"]]["primitives"]:
                acc = gltf["accessors"][prim["attributes"]["POSITION"]]
                lo, hi = acc["min"], acc["max"]
                pts = [
                    transform_point(world, (x, y, z))
                    for x in (lo[0], hi[0])
                    for y in (lo[1], hi[1])
                    for z in (lo[2], hi[2])
                ]
                boxes.append((
                    node.get("name", ""),
                    tuple(min(p[i] for p in pts) for i in range(3)),
                    tuple(max(p[i] for p in pts) for i in range(3)),
                ))
        for child in node.get("children", []):
            visit(child, world)

    scene = gltf["scenes"][gltf.get("scene", 0)]
    for root in scene["nodes"]:
        visit(root, identity())
    return boxes


def is_floor(name):
    return name.startswith("Tile") or name.startswith("Floor")


def compute_center_xz(gltf, mode):
    """中央にしたい XZ 座標を返す。見つからなければ None。"""
    boxes = collect_world_boxes(gltf)
    if mode == "floor":
        boxes = [b for b in boxes if is_floor(b[0])]
    if not boxes:
        return None
    min_x = min(b[1][0] for b in boxes)
    max_x = max(b[2][0] for b in boxes)
    min_z = min(b[1][2] for b in boxes)
    max_z = max(b[2][2] for b in boxes)
    return ((min_x + max_x) / 2, (min_z + max_z) / 2)


# ---------------------------------------------------------------- 移動の適用

def shift_scene_roots(gltf, dx, dz):
    """
    シーン直下のノードを (dx, 0, dz) だけ動かす。
    ルートが回転していても、ワールド空間で同じだけ動くように
    「translation から引く」だけでよい(T*R*S の T 部分だけが変わるため)。
    """
    scene = gltf["scenes"][gltf.get("scene", 0)]
    for index in scene["nodes"]:
        node = gltf["nodes"][index]
        if "matrix" in node:
            node["matrix"][12] = clean(node["matrix"][12] + dx)
            node["matrix"][14] = clean(node["matrix"][14] + dz)
        else:
            t = node.get("translation", [0, 0, 0])
            node["translation"] = [clean(t[0] + dx), t[1], clean(t[2] + dz)]


def clean(v):
    """浮動小数点の誤差(0.0000001 など)を丸める"""
    v = round(v, 6)
    return 0.0 if v == 0 else v


# ---------------------------------------------------------------- ファイル処理

def process_file(path, args):
    with open(path, "r", encoding="utf-8") as f:
        gltf = json.load(f)

    center = compute_center_xz(gltf, args.mode)
    if center is None:
        print(f"[スキップ] {path}: 床ノード(Tile*/Floor*)が見つかりません")
        return
    cx, cz = center
    if abs(cx) < EPS and abs(cz) < EPS:
        print(f"[済み]     {path}: すでに中央が原点です")
        return

    print(f"[変換]     {path}: 中央=({cx:.3f}, {cz:.3f}) → (0, 0)  / 移動量 X={-cx:.3f}, Z={-cz:.3f}")
    if args.dry_run:
        return

    shift_scene_roots(gltf, -cx, -cz)

    base, ext = os.path.splitext(path)
    if args.inplace:
        shutil.copyfile(path, path + ".orig")
        out_path = path
    else:
        out_path = base + args.suffix + ext

    # Blender の出力に合わせて、タブ字下げ・コンパクトな区切りで書き出す
    with open(out_path, "w", encoding="utf-8", newline="\n") as f:
        json.dump(gltf, f, indent="\t", separators=(",", ":"), ensure_ascii=False)
    print(f"           → {out_path}")


def find_gltf_files(target, suffix):
    if os.path.isfile(target):
        yield target
        return
    for root, _dirs, files in os.walk(target):
        for name in sorted(files):
            stem, ext = os.path.splitext(name)
            if ext.lower() == ".gltf" and not stem.endswith(suffix):
                yield os.path.join(root, name)


def main():
    parser = argparse.ArgumentParser(description="glTF の原点を床の中央(XZ)に移動する")
    parser.add_argument("targets", nargs="+", help=".gltf ファイル、またはフォルダ")
    parser.add_argument("--suffix", default="_Centered", help="出力ファイル名の末尾")
    parser.add_argument("--inplace", action="store_true", help="元ファイルを上書き(*.orig に退避)")
    parser.add_argument("--mode", choices=["floor", "all"], default="floor",
                        help="floor: 床ノードの中心 / all: 全ジオメトリの中心")
    parser.add_argument("--dry-run", action="store_true", help="書き込まず移動量だけ表示")
    args = parser.parse_args()

    for target in args.targets:
        if not os.path.exists(target):
            print(f"見つかりません: {target}", file=sys.stderr)
            continue
        for path in find_gltf_files(target, args.suffix):
            process_file(path, args)


if __name__ == "__main__":
    main()
