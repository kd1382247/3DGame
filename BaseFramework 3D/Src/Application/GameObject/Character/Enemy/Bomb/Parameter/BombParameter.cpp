#include "BombParameter.h"

#include"../../../../../Editor/EditorManager.h"
#include"../../../../../Editor/InspectorWidgets.h"
#include"../../../ParameterJson.h"

#include"json.hpp"
#include<fstream>


namespace
{
	// Parameter → json
	nlohmann::json ToJson(const BombParameter::Parameter& param)
	{
		nlohmann::json json;

		json["MaxHP"] = param.m_maxHP;
		json["BumpSphereRadius"] = InspectorUI::CeilToDigits(param.m_bumpSphereRadius, 2);
		json["BumpSphereOffsetY"] = InspectorUI::CeilToDigits(param.m_bumpSphereOffsetY, 2);
		json["MoveSpeed"] = InspectorUI::CeilToDigits(param.m_moveSpeed, 3);
		json["TurnSpeed"] = InspectorUI::CeilToDigits(param.m_turnSpeed, 1);
		json["ReachDistance"] = InspectorUI::CeilToDigits(param.m_reachDistance, 2);
		json["JumpPower"] = InspectorUI::CeilToDigits(param.m_jumpPow, 3);
		json["AttackPower"] = InspectorUI::CeilToDigits(param.m_attackPow, 1);
		json["AttackCooldown"] = InspectorUI::CeilToDigits(param.m_attackCooldown, 2);
		json["ExplosionRadius"] = InspectorUI::CeilToDigits(param.m_explosionRadius, 2);
		json["ChargeDuration"] = InspectorUI::CeilToDigits(param.m_chargeDuration, 2);
		json["ExplosionKnockBack"] = InspectorUI::CeilToDigits(param.m_explosionKnockBack, 3);

		return json;
	}

	// json → Parameter(キーが無い項目は、デフォルト値のまま)
	void FromJson(const nlohmann::json& json, BombParameter::Parameter& param)
	{
		ParameterJson::Read(json, "MaxHP", param.m_maxHP);
		ParameterJson::Read(json, "BumpSphereRadius", param.m_bumpSphereRadius);
		ParameterJson::Read(json, "BumpSphereOffsetY", param.m_bumpSphereOffsetY);
		ParameterJson::Read(json, "MoveSpeed", param.m_moveSpeed);
		ParameterJson::Read(json, "TurnSpeed", param.m_turnSpeed);
		ParameterJson::Read(json, "ReachDistance", param.m_reachDistance);
		ParameterJson::Read(json, "JumpPower", param.m_jumpPow);
		ParameterJson::Read(json, "AttackPower", param.m_attackPow);
		ParameterJson::Read(json, "AttackCooldown", param.m_attackCooldown);
		ParameterJson::Read(json, "ExplosionRadius", param.m_explosionRadius);
		ParameterJson::Read(json, "ChargeDuration", param.m_chargeDuration);
		ParameterJson::Read(json, "ExplosionKnockBack", param.m_explosionKnockBack);
	}
}

void BombParameter::Init()
{
	LoadFromJson();
}

namespace
{
	// 1つ分のパラメータをグループごとに描画する
	void DrawParameter_(BombParameter::Parameter& param)
	{
		if (InspectorUI::BeginGroup("Status"))
		{
			InspectorUI::HP("MaxHP", param.m_maxHP);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Collision"))
		{
			InspectorUI::Meter("BumpSphereRadius", param.m_bumpSphereRadius);
			InspectorUI::Meter("BumpSphereOffsetY", param.m_bumpSphereOffsetY);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Move"))
		{
			InspectorUI::MeterPerFrame("MoveSpeed", param.m_moveSpeed);
			InspectorUI::DegreePerFrame("TurnSpeed", param.m_turnSpeed);
			InspectorUI::Meter("ReachDistance", param.m_reachDistance);
			InspectorUI::Power("JumpPower", param.m_jumpPow);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Attack"))
		{
			InspectorUI::Damage("AttackPower", param.m_attackPow);
			InspectorUI::Seconds("AttackCooldown", param.m_attackCooldown);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Explosion"))
		{
			InspectorUI::Meter("Radius", param.m_explosionRadius);
			InspectorUI::Seconds("ChargeDuration", param.m_chargeDuration);
			InspectorUI::Power("KnockBack", param.m_explosionKnockBack);

			InspectorUI::EndGroup();
		}
	}
}

void BombParameter::DrawInspecter()
{
	if (ImGui::CollapsingHeader("Parameter", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 単位の説明(m / s / F / ° など)
		InspectorUI::UnitLegend();

		// セーブ
		if (ImGui::Button("SaveParameter"))
		{
			SaveToJson();
		}

		DrawParameter_(m_param);
	}
}

void BombParameter::SaveToJson()
{
	nlohmann::json paramJson = ToJson(m_param);

	std::ofstream file("Asset/Data/Enemy/Bomb/Parameter/BombParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("Bomb parameter save filed\n");
		KdDebugGUI::Instance().AddErrorLog("Bomb parameter save filed\n");

		return;
	}
}

void BombParameter::LoadFromJson()
{

	std::fstream file("Asset/Data/Enemy/Bomb/Parameter/BombParameter.json");

	// もしファイルを開けないとき(デフォルト値のまま動く)
	if (!file.is_open())
	{
		OutputDebugStringA("BombのParameter.jsonを開けませんでした\n");
		KdDebugGUI::Instance().AddErrorLog("BombのParameter.jsonを開けませんでした\n");
		return;
	}

	nlohmann::json paramJson;

	try
	{
		file >> paramJson;

		FromJson(paramJson, m_param);
	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA("JSONの読み込みに失敗しました\n");
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		KdDebugGUI::Instance().AddErrorLog("JSONの読み込みに失敗しました\n");
		KdDebugGUI::Instance().AddErrorLog("%s\n", e.what());
		return;
	}
}
