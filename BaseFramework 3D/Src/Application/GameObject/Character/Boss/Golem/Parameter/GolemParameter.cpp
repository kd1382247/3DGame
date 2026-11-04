#include "GolemParameter.h"

#include"../../../../../Editor/EditorManager.h"
#include"../../../../../Editor/InspectorWidgets.h"
#include"../../../ParameterJson.h"

#include"json.hpp"
#include<fstream>


namespace
{
	// Parameter → json
	nlohmann::json ToJson(const GolemParameter::Parameter& param)
	{
		nlohmann::json json;

		json["MaxHP"] = param.m_maxHP;
		json["BumpSphereRadius"] = InspectorUI::CeilToDigits(param.m_bumpSphereRadius, 2);
		json["BumpSphereOffsetY"] = InspectorUI::CeilToDigits(param.m_bumpSphereOffsetY, 2);
		json["Scale"] = InspectorUI::CeilToDigits(param.m_scale, 2);
		json["MoveSpeed"] = InspectorUI::CeilToDigits(param.m_moveSpeed, 3);
		json["TurnSpeed"] = InspectorUI::CeilToDigits(param.m_turnSpeed, 1);
		json["ReachDistance"] = InspectorUI::CeilToDigits(param.m_reachDistance, 2);
		json["ReachDistanceMargin"] = InspectorUI::CeilToDigits(param.m_reachDistanceMargin, 2);
		json["JumpPower"] = InspectorUI::CeilToDigits(param.m_jumpPow, 3);
		json["AttackPower"] = InspectorUI::CeilToDigits(param.m_attackPow, 1);

		return json;
	}

	// json → Parameter(キーが無い項目は、デフォルト値のまま)
	void FromJson(const nlohmann::json& json, GolemParameter::Parameter& param)
	{
		ParameterJson::Read(json, "MaxHP", param.m_maxHP);
		ParameterJson::Read(json, "BumpSphereRadius", param.m_bumpSphereRadius);
		ParameterJson::Read(json, "BumpSphereOffsetY", param.m_bumpSphereOffsetY);
		ParameterJson::Read(json, "Scale", param.m_scale);
		ParameterJson::Read(json, "MoveSpeed", param.m_moveSpeed);
		ParameterJson::Read(json, "TurnSpeed", param.m_turnSpeed);
		ParameterJson::Read(json, "ReachDistance", param.m_reachDistance);
		ParameterJson::Read(json, "ReachDistanceMargin", param.m_reachDistanceMargin);
		ParameterJson::Read(json, "JumpPower", param.m_jumpPow);
		ParameterJson::Read(json, "AttackPower", param.m_attackPow);
	}
}

void GolemParameter::Init()
{
	LoadFromJson();
}

namespace
{
	// 1つ分のパラメータをグループごとに描画する
	void DrawParameter_(GolemParameter::Parameter& param)
	{
		if (InspectorUI::BeginGroup("Status"))
		{
			InspectorUI::HP("MaxHP", param.m_maxHP);
			InspectorUI::Scale("Scale", param.m_scale);

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
			InspectorUI::Meter("ReachDistanceMargin", param.m_reachDistanceMargin);
			InspectorUI::Power("JumpPower", param.m_jumpPow);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Attack"))
		{
			InspectorUI::Damage("AttackPower", param.m_attackPow);

			InspectorUI::EndGroup();
		}
	}
}

void GolemParameter::DrawInspecter()
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

void GolemParameter::SaveToJson()
{
	nlohmann::json paramJson = ToJson(m_param);

	std::ofstream file("Asset/Data/Enemy/Golem/Parameter/GolemParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("Golem parameter save filed\n");
		KdDebugGUI::Instance().AddErrorLog("Golem parameter save filed\n");

		return;
	}
}

void GolemParameter::LoadFromJson()
{

	std::fstream file("Asset/Data/Enemy/Golem/Parameter/GolemParameter.json");

	// もしファイルを開けないとき(デフォルト値のまま動く)
	if (!file.is_open())
	{
		OutputDebugStringA("GolemのParameter.jsonを開けませんでした\n");
		KdDebugGUI::Instance().AddErrorLog("GolemのParameter.jsonを開けませんでした\n");
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
