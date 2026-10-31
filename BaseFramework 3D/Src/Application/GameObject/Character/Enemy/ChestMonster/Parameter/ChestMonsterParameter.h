#pragma once

#include <set>
#include <string>
#include"json.hpp"

class ChestMonsterParameter
{
public:

	// 値の単位は Inspector に表示される。m=メートル s=秒 F=フレーム(60F=1秒) °=度 m/F=1フレームの移動量
	struct Parameter
	{
		//---- Status ----
		int m_maxHP = 100;   // [HP] 最大HP
		float m_scale = 2.0f;   // [倍] 大きさ(倍率)

		//---- Move ----
		float m_moveSpeed = 0.15f;   // [m/F] 移動速度(1フレームの移動量。実際の速さは 値 x 60 m/秒)
		float m_turnSpeed = 12.0f;   // [°/F] 回転速度(1フレームに回転できる最大角度)
		float m_reachDistance = 1.5f;   // [m] 到達判定の距離(この距離まで近づいたら攻撃を開始する)
		float m_jumpPow = 0.4f;   // ジャンプ力(飛び出しの勢い)

		//---- Attack ----
		float m_attackPow = 10.0f;   // 攻撃力

		//---- Spawn ----
		float m_spawnInterval = 2.0f;   // [秒] 敵を呼び出したあと、次に呼び出せるようになるまでの時間
		float m_spawnStartFrame = 40.0f;   // [F] 敵が出始めるフレーム(アニメーション開始から)
		float m_spawnEndFrame = 60.0f;   // [F] 敵が出なくなるフレーム(アニメーション開始から)
		float m_spawnWaitFrame = 8.0f;   // [F] 敵を1体出したあと、次の1体までの待ちフレーム
		int   m_maxSpawnedAlive = 10;   // [体] このChestMonsterが出した敵が、同時に生きていられる数(この数なら召喚しない)
		int   m_maxTotalEnemies = 40;   // [体] 全体の敵の数の上限(ChestMonster以外の生きている敵。この数なら召喚しない)

		//---- Spawned Enemy ----
		std::set<std::string> m_spawnEnemyNames = { "Slime", "Mushroom", "TurtleShell" };   // 召喚する敵の名前(この中からランダムに選ぶ)

		float m_spawnHeight = 2.0f;   // [m] 敵を出す高さ(自分の位置から)
		float m_spawnLaunchPower = 0.2f;   // 出した敵が飛び上がる勢い
		float m_spawnLaunchSpeed = 0.04f;   // [m/F] 出した敵が正面へ飛び出す速さ

		//---- Collision ----
		float m_bumpSphereRadius = 0.8f;   // [m] 押し戻し判定の球の半径
		float m_bumpSphereOffsetY = 0.8f;   // [m] 足元から、押し戻し判定の球の中心までの高さ(地面に立たせるなら半径と同じ値)
	};

	const Parameter& GetParam()const { return m_param; }

	void Init();

	void DrawInspecter();

	// ステージデータ(1体ごとの個別パラメータ)用の入出力
	nlohmann::json ToJson()const;
	void FromJson(const nlohmann::json& json);


private:

	void SaveToJson();
	void LoadFromJson();

	Parameter m_param = {};
};
