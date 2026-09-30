#include "TurtleShellParameter.h"

#include"../../../../../Editor/EditorManager.h"
#include"../../../../../Editor/InspectorWidgets.h"
#include"../../../ParameterJson.h"

#include"json.hpp"
#include<fstream>


namespace
{
	// Parameter → json
	nlohmann::json ToJson(const TurtleShellParameter::Parameter& param)
	{
		nlohmann::json json;

		json["MaxHP"] = param.m_maxHP;
		json["MoveSpeed"] = param.m_moveSpeed;
		json["TurnSpeed"] = param.m_turnSpeed;
		json["ReachDistance"] = param.m_reachDistance;
		json["JumpPower"] = param.m_jumpPow;
		json["AttackPower"] = param.m_attackPow;
		json["AttackCooldown"] = param.m_attackCooldown;
		json["KnockBackPower"] = param.m_knockBackPower;
		json["SpinDuration"] = param.m_spinDuration;
		json["SpinSpeedBonus"] = param.m_spinSpeedBonus;
		json["SpinHitRadius"] = param.m_spinHitRadius;
		json["SpinHitCooldown"] = param.m_spinHitCooldown;
		json["DizzyDuration"] = param.m_dizzyDuration;

		return json;
	}

	// json → Parameter(キーが無い項目は、デフォルト値のまま)
	void FromJson(const nlohmann::json& json, TurtleShellParameter::Parameter& param)
	{
		ParameterJson::Read(json, "MaxHP", param.m_maxHP);
		ParameterJson::Read(json, "MoveSpeed", param.m_moveSpeed);
		ParameterJson::Read(json, "TurnSpeed", param.m_turnSpeed);
		ParameterJson::Read(json, "ReachDistance", param.m_reachDistance);
		ParameterJson::Read(json, "JumpPower", param.m_jumpPow);
		ParameterJson::Read(json, "AttackPower", param.m_attackPow);
		ParameterJson::Read(json, "AttackCooldown", param.m_attackCooldown);
		ParameterJson::Read(json, "KnockBackPower", param.m_knockBackPower);
		ParameterJson::Read(json, "SpinDuration", param.m_spinDuration);
		ParameterJson::Read(json, "SpinSpeedBonus", param.m_spinSpeedBonus);
		ParameterJson::Read(json, "SpinHitRadius", param.m_spinHitRadius);
		ParameterJson::Read(json, "SpinHitCooldown", param.m_spinHitCooldown);
		ParameterJson::Read(json, "DizzyDuration", param.m_dizzyDuration);
	}
}

void TurtleShellParameter::Init()
{
	LoadFromJson();
}

namespace
{
	// 1つ分のパラメータをグループごとに描画する
	void DrawParameter_(TurtleShellParameter::Parameter& param)
	{
		if (InspectorUI::BeginGroup("Status"))
		{
			InspectorUI::HP("MaxHP", param.m_maxHP);

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
			InspectorUI::Power("KnockBackPower", param.m_knockBackPower);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Spin Attack"))
		{
			InspectorUI::Seconds("Duration", param.m_spinDuration);
			InspectorUI::MeterPerFrame("SpeedBonus", param.m_spinSpeedBonus);
			InspectorUI::Meter("HitRadius", param.m_spinHitRadius);
			InspectorUI::Seconds("HitCooldown", param.m_spinHitCooldown);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Dizzy"))
		{
			InspectorUI::Seconds("Duration", param.m_dizzyDuration);

			InspectorUI::EndGroup();
		}
	}
}

void TurtleShellParameter::DrawInspecter()
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

void TurtleShellParameter::SaveToJson()
{
	nlohmann::json paramJson = ToJson(m_param);

	std::ofstream file("Asset/Data/Enemy/TurtleShell/Parameter/TurtleShellParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("TurtleShell parameter save filed\n");
		KdDebugGUI::Instance().AddErrorLog("TurtleShell parameter save filed\n");

		return;
	}
}

void TurtleShellParameter::LoadFromJson()
{

	std::fstream file("Asset/Data/Enemy/TurtleShell/Parameter/TurtleShellParameter.json");

	// もしファイルを開けないとき(デフォルト値のまま動く)
	if (!file.is_open())
	{
		OutputDebugStringA("TurtleShellのParameter.jsonを開けませんでした\n");
		KdDebugGUI::Instance().AddErrorLog("TurtleShellのParameter.jsonを開けませんでした\n");
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
