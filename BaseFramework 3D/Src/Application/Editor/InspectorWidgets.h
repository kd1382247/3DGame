#pragma once

#include <cfloat>
#include <climits>

#include "EditorManager.h"

//==================================================================
// Inspectorに表示する「単位つきの入力欄」と「グループ見出し」の部品
// (各Parameterクラスの DrawInspecter から使う)
//
// 値の右側に単位を表示して、何の値かが一目で分かるようにする
//
//   m     … メートル(距離・半径・長さ)
//   s     … 秒
//   F     … フレーム(60fps換算。アニメーションの何コマ目か)
//   °     … 度(角度)
//   m/F   … 1フレームあたりの移動量(実際の速さは 値 x 60 m/秒)
//   °/F   … 1フレームあたりの回転量
//
// 値を変更したら、自動的に EditorManager::MarkDirty() を呼ぶ
//==================================================================
namespace InspectorUI
{
	// 度(°)はUTF-8で書く(ソースの文字コードに左右されないよう、バイト列で指定)
	// ※"\xB0"の後ろに16進数の文字が続くと巻き込まれるため、文字列を分けて連結している
	constexpr const char* kFormatDegree         = "%.1f\xC2\xB0";
	constexpr const char* kFormatDegreePerFrame = "%.1f\xC2\xB0" "/F";

	// 値が変更されていたらMarkDirty()して、変更されたかどうかをそのまま返す
	inline bool Notify(const bool changed)
	{
		if (changed)
		{
			EditorManager::Instance().MarkDirty();
		}

		return changed;
	}

	//------------------------------------------------------------
	// グループ見出し(クリックで開閉できる)
	// trueを返した時だけ中身を描き、最後に必ずEndGroup()を呼ぶ
	//
	//   if (InspectorUI::BeginGroup("Move"))
	//   {
	//       InspectorUI::MeterPerFrame("MoveSpeed", value);
	//       InspectorUI::EndGroup();
	//   }
	//------------------------------------------------------------
	inline bool BeginGroup(const char* title)
	{
		return ImGui::TreeNodeEx(title, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed);
	}

	inline void EndGroup()
	{
		ImGui::TreePop();
	}

	//------------------------------------------------------------
	// 単位の凡例(Parameterの先頭に1行だけ薄く表示する)
	// ※"°"が「?」と表示される場合は、kFormatDegree / kFormatDegreePerFrame を "deg" 表記に変えればよい
	//------------------------------------------------------------
	inline void UnitLegend()
	{
		ImGui::TextDisabled("m: meter   s: second   F: frame (60F = 1s)");
		ImGui::TextDisabled("m/F: meter per frame   %s: degree", "\xC2\xB0");
	}

	//------------------------------------------------------------
	// 単位を指定できる汎用の入力欄(通常は下の単位別の関数を使う)
	//------------------------------------------------------------
	inline bool Float(const char* label, float& value, const float speed, const float minValue, const char* format)
	{
		return Notify(ImGui::DragFloat(label, &value, speed, minValue, FLT_MAX, format));
	}

	//------------------------------------------------------------
	// 単位別の入力欄
	//------------------------------------------------------------

	// 距離・半径・長さ(m)
	inline bool Meter(const char* label, float& value)          { return Float(label, value, 0.05f, 0.0f, "%.2f m"); }

	// 1フレームあたりの移動量(m/F)
	inline bool MeterPerFrame(const char* label, float& value)  { return Float(label, value, 0.005f, 0.0f, "%.3f m/F"); }

	// 時間(秒)
	inline bool Seconds(const char* label, float& value)        { return Float(label, value, 0.05f, 0.0f, "%.2f s"); }

	// フレーム数・フレーム番号(F)
	inline bool Frame(const char* label, float& value)          { return Float(label, value, 0.5f, 0.0f, "%.1f F"); }

	// 角度(°)
	inline bool Degree(const char* label, float& value)         { return Float(label, value, 1.0f, 0.0f, kFormatDegree); }

	// 1フレームあたりの回転量(°/F)
	inline bool DegreePerFrame(const char* label, float& value) { return Float(label, value, 0.05f, 0.0f, kFormatDegreePerFrame); }

	// 重力加速度(m/s^2)
	inline bool Acceleration(const char* label, float& value)   { return Float(label, value, 0.1f, 0.0f, "%.1f m/s^2"); }

	// ダメージ量(単位なしの数値)
	inline bool Damage(const char* label, float& value)         { return Float(label, value, 1.0f, 0.0f, "%.1f"); }

	// ノックバック・ジャンプ力など、小さい数値(単位なし)
	inline bool Power(const char* label, float& value)          { return Float(label, value, 0.005f, 0.0f, "%.3f"); }

	// 倍率(x)
	inline bool Scale(const char* label, float& value)          { return Float(label, value, 0.01f, 0.0001f, "%.2f x"); }

	// 確率などの百分率(0〜100 %)
	inline bool Percent(const char* label, float& value)        { return Notify(ImGui::DragFloat(label, &value, 0.5f, 0.0f, 100.0f, "%.1f %%")); }

	// 0.0〜1.0の割合
	inline bool Rate(const char* label, float& value)           { return Notify(ImGui::DragFloat(label, &value, 0.01f, 0.0f, 1.0f, "%.2f")); }

	// HP
	inline bool HP(const char* label, int& value)               { return Notify(ImGui::DragInt(label, &value, 1.0f, 0, INT_MAX, "%d HP")); }

	// 個数・回数
	inline bool Count(const char* label, int& value, const int minValue = 0)
	{
		return Notify(ImGui::DragInt(label, &value, 0.2f, minValue, INT_MAX, "%d"));
	}
}
