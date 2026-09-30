#include "SlimeParameter.h"

#include"../../../../../Editor/EditorManager.h"
#include"../../../../../Editor/InspectorWidgets.h"
#include"../../../ParameterJson.h"

#include"json.hpp"
#include<fstream>


namespace
{
	// Parameter → json
	nlohmann::json ToJson(const SlimeParameter::Parameter& param)
	{
		nlohmann::json json;

		json["MaxHP"] = param.m_maxHP;
		json["Scale"] = param.m_scale;
		json["MoveSpeed"] = param.m_moveSpeed;
		json["TurnSpeed"] = param.m_turnSpeed;
		json["ReachDistance"] = param.m_reachDistance;
		json["JumpPower"] = param.m_jumpPow;
		json["AttackPower"] = param.m_attackPower;
		json["AttackCooldown"] = param.m_attackCooldown;
		json["KnockBackPower"] = param.m_knockBackPower;
		json["HitStartFrame"] = param.m_hitStartFrame;
		json["HitEndFrame"] = param.m_hitEndFrame;
		json["HitRadius"] = param.m_hitRadius;
		json["HitForwardOffset"] = param.m_hitForwardOffset;

		return json;
	}

	// json → Parameter(キーが無い項目は、デフォルト値のまま)
	void FromJson(const nlohmann::json& json, SlimeParameter::Parameter& param)
	{
		ParameterJson::Read(json, "MaxHP", param.m_maxHP);
		ParameterJson::Read(json, "Scale", param.m_scale);
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

SlimeParameter::SlimeParameter()
{
	// 大きいスライムは、大きさ(Scale)だけデフォルト値を大きくしておく
	m_paramLarge.m_scale = 1.5f;
}

const SlimeParameter::Parameter& SlimeParameter::GetParam(const SlimeSize size) const
{
	if (size == SlimeSize::Large)
	{
		return m_paramLarge;
	}

	return m_paramSmall;
}

void SlimeParameter::Init()
{
	LoadFromJson();
}

void SlimeParameter::DrawInspecter()
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

		if (ImGui::TreeNodeEx("Large", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawParameter(m_paramLarge);

			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Small", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawParameter(m_paramSmall);

			ImGui::TreePop();
		}

		if (InspectorUI::BeginGroup("Split"))
		{
			InspectorUI::MeterPerFrame("SplitLaunchSpeed", m_splitLaunchSpeed);
			InspectorUI::Power("SplitLaunchPower", m_splitLaunchPower);

			InspectorUI::EndGroup();
		}
	}
}

void SlimeParameter::DrawParameter(Parameter& param)
{
	if (InspectorUI::BeginGroup("Status"))
	{
		InspectorUI::HP("MaxHP", param.m_maxHP);
		InspectorUI::Scale("Scale", param.m_scale);

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

void SlimeParameter::SaveToJson()
{
	nlohmann::json paramJson;

	paramJson["Large"] = ToJson(m_paramLarge);
	paramJson["Small"] = ToJson(m_paramSmall);

	paramJson["SplitLaunchSpeed"] = m_splitLaunchSpeed;
	paramJson["SplitLaunchPower"] = m_splitLaunchPower;

	std::ofstream file("Asset/Data/Enemy/Slime/Parameter/SlimeParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("Slime parameter save filed\n");
		KdDebugGUI::Instance().AddErrorLog("Slime parameter save filed\n");

		return;
	}
}

void SlimeParameter::LoadFromJson()
{

	std::fstream file("Asset/Data/Enemy/Slime/Parameter/SlimeParameter.json");

	// もしファイルを開けないとき(デフォルト値のまま動く)
	if (!file.is_open())
	{
		OutputDebugStringA("SlimeのParameter.jsonを開けませんでした\n");
		KdDebugGUI::Instance().AddErrorLog("SlimeのParameter.jsonを開けませんでした\n");
		return;
	}

	nlohmann::json paramJson;

	try
	{
		file >> paramJson;

		if (paramJson.contains("Large"))
		{
			FromJson(paramJson["Large"], m_paramLarge);
		}

		if (paramJson.contains("Small"))
		{
			FromJson(paramJson["Small"], m_paramSmall);
		}

		ParameterJson::Read(paramJson, "SplitLaunchSpeed", m_splitLaunchSpeed);
		ParameterJson::Read(paramJson, "SplitLaunchPower", m_splitLaunchPower);
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
