#include "CactasParameter.h"

#include"../../../../../Editor/EditorManager.h"
#include"../../../../../Editor/InspectorWidgets.h"
#include"../../../ParameterJson.h"

#include"json.hpp"
#include<fstream>


namespace
{
	// Parameter → json
	nlohmann::json ToJson(const CactasParameter::Parameter& param)
	{
		nlohmann::json json;

		json["MaxHP"] = param.m_maxHP;
		json["BumpSphereRadius"] = InspectorUI::CeilToDigits(param.m_bumpSphereRadius, 2);
		json["BumpSphereOffsetY"] = InspectorUI::CeilToDigits(param.m_bumpSphereOffsetY, 2);
		json["MoveSpeed"] = InspectorUI::CeilToDigits(param.m_moveSpeed, 3);
		json["TurnSpeed"] = InspectorUI::CeilToDigits(param.m_turnSpeed, 1);
		json["ReachDistance"] = InspectorUI::CeilToDigits(param.m_reachDistance, 2);
		json["JumpPower"] = InspectorUI::CeilToDigits(param.m_jumpPow, 3);
		json["AttackPower"] = InspectorUI::CeilToDigits(param.m_attackPower, 1);
		json["AttackCooldown"] = InspectorUI::CeilToDigits(param.m_attackCooldown, 2);
		json["KnockBackPower"] = InspectorUI::CeilToDigits(param.m_knockBackPower, 3);
		json["HitStartFrame"] = InspectorUI::CeilToDigits(param.m_hitStartFrame, 1);
		json["HitEndFrame"] = InspectorUI::CeilToDigits(param.m_hitEndFrame, 1);
		json["HitRadius"] = InspectorUI::CeilToDigits(param.m_hitRadius, 2);
		json["HitForwardOffset"] = InspectorUI::CeilToDigits(param.m_hitForwardOffset, 2);

		return json;
	}

	// json → Parameter(キーが無い項目は、デフォルト値のまま)
	void FromJson(const nlohmann::json& json, CactasParameter::Parameter& param)
	{
		ParameterJson::Read(json, "MaxHP", param.m_maxHP);
		ParameterJson::Read(json, "BumpSphereRadius", param.m_bumpSphereRadius);
		ParameterJson::Read(json, "BumpSphereOffsetY", param.m_bumpSphereOffsetY);
		ParameterJson::Read(json, "MoveSpeed", param.m_moveSpeed);
		ParameterJson::Read(json, "TurnSpeed", param.m_turnSpeed);
		ParameterJson::Read(json, "ReachDistance", param.m_reachDistance);
		ParameterJson::Read(json, "JumpPower", param.m_jumpPow);
		ParameterJson::Read(json, "AttackPower", param.m_attackPower);
		ParameterJson::Read(json, "AttackCooldown", param.m_attackCooldown);
		ParameterJson::Read(json, "KnockBackPower", param.m_knockBackPower);
		ParameterJson::Read(json, "HitStartFrame", param.m_hitStartFrame);
		ParameterJson::Read(json, "HitEndFrame", param.m_hitEndFrame);
		ParameterJson::Read(json, "HitRadius", param.m_hitRadius);
		ParameterJson::Read(json, "HitForwardOffset", param.m_hitForwardOffset);
	}
}

void CactasParameter::Init()
{
	LoadFromJson();
}

namespace
{
	// 1つ分のパラメータをグループごとに描画する
	void DrawParameter_(CactasParameter::Parameter& param)
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
			InspectorUI::Damage("AttackPower", param.m_attackPower);
			InspectorUI::Seconds("AttackCooldown", param.m_attackCooldown);
			InspectorUI::Power("KnockBackPower", param.m_knockBackPower);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Attack Hit"))
		{
			InspectorUI::Frame("HitStartFrame", param.m_hitStartFrame);
			InspectorUI::Frame("HitEndFrame", param.m_hitEndFrame);
			InspectorUI::Meter("HitRadius", param.m_hitRadius);
			InspectorUI::Meter("HitForwardOffset", param.m_hitForwardOffset);

			InspectorUI::EndGroup();
		}
	}
}

void CactasParameter::DrawInspecter()
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

void CactasParameter::SaveToJson()
{
	nlohmann::json paramJson = ToJson(m_param);

	std::ofstream file("Asset/Data/Enemy/Cactas/Parameter/CactasParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("Cactas parameter save filed\n");
		KdDebugGUI::Instance().AddErrorLog("Cactas parameter save filed\n");

		return;
	}
}

void CactasParameter::LoadFromJson()
{

	std::fstream file("Asset/Data/Enemy/Cactas/Parameter/CactasParameter.json");

	// もしファイルを開けないとき(デフォルト値のまま動く)
	if (!file.is_open())
	{
		OutputDebugStringA("CactasのParameter.jsonを開けませんでした\n");
		KdDebugGUI::Instance().AddErrorLog("CactasのParameter.jsonを開けませんでした\n");
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
