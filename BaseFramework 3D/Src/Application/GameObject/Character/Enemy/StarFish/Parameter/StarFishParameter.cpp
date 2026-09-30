#include "StarFishParameter.h"

#include"../../../../../Editor/EditorManager.h"
#include"../../../../../Editor/InspectorWidgets.h"
#include"../../../ParameterJson.h"

#include"json.hpp"
#include<fstream>


namespace
{
	// Parameter → json
	nlohmann::json ToJson(const StarFishParameter::Parameter& param)
	{
		nlohmann::json json;

		json["MaxHP"] = param.m_maxHP;
		json["MoveSpeed"] = param.m_moveSpeed;
		json["TurnSpeed"] = param.m_turnSpeed;
		json["ReachDistance"] = param.m_reachDistance;
		json["JumpPower"] = param.m_jumpPow;
		json["AttackPower"] = param.m_attackPow;
		json["AttackCooldown"] = param.m_attackCooldown;
		json["BulletFireFrame"] = param.m_bulletFireFrame;
		json["BulletSpeed"] = param.m_bulletSpeed;
		json["BulletRadius"] = param.m_bulletRadius;
		json["BulletKnockBack"] = param.m_bulletKnockBack;
		json["BulletLifeTime"] = param.m_bulletLifeTime;
		json["BulletSpawnHeight"] = param.m_bulletSpawnHeight;
		json["BulletSpawnForward"] = param.m_bulletSpawnForward;
		json["BulletAimHeight"] = param.m_bulletAimHeight;

		return json;
	}

	// json → Parameter(キーが無い項目は、デフォルト値のまま)
	void FromJson(const nlohmann::json& json, StarFishParameter::Parameter& param)
	{
		ParameterJson::Read(json, "MaxHP", param.m_maxHP);
		ParameterJson::Read(json, "MoveSpeed", param.m_moveSpeed);
		ParameterJson::Read(json, "TurnSpeed", param.m_turnSpeed);
		ParameterJson::Read(json, "ReachDistance", param.m_reachDistance);
		ParameterJson::Read(json, "JumpPower", param.m_jumpPow);
		ParameterJson::Read(json, "AttackPower", param.m_attackPow);
		ParameterJson::Read(json, "AttackCooldown", param.m_attackCooldown);
		ParameterJson::Read(json, "BulletFireFrame", param.m_bulletFireFrame);
		ParameterJson::Read(json, "BulletSpeed", param.m_bulletSpeed);
		ParameterJson::Read(json, "BulletRadius", param.m_bulletRadius);
		ParameterJson::Read(json, "BulletKnockBack", param.m_bulletKnockBack);
		ParameterJson::Read(json, "BulletLifeTime", param.m_bulletLifeTime);
		ParameterJson::Read(json, "BulletSpawnHeight", param.m_bulletSpawnHeight);
		ParameterJson::Read(json, "BulletSpawnForward", param.m_bulletSpawnForward);
		ParameterJson::Read(json, "BulletAimHeight", param.m_bulletAimHeight);
	}
}

void StarFishParameter::Init()
{
	LoadFromJson();
}

namespace
{
	// 1つ分のパラメータをグループごとに描画する
	void DrawParameter_(StarFishParameter::Parameter& param)
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

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Bullet"))
		{
			InspectorUI::Frame("FireFrame", param.m_bulletFireFrame);
			InspectorUI::MeterPerFrame("Speed", param.m_bulletSpeed);
			InspectorUI::Meter("Radius", param.m_bulletRadius);
			InspectorUI::Power("KnockBack", param.m_bulletKnockBack);
			InspectorUI::Seconds("LifeTime", param.m_bulletLifeTime);
			InspectorUI::Meter("SpawnHeight", param.m_bulletSpawnHeight);
			InspectorUI::Meter("SpawnForward", param.m_bulletSpawnForward);
			InspectorUI::Meter("AimHeight", param.m_bulletAimHeight);

			InspectorUI::EndGroup();
		}
	}
}

void StarFishParameter::DrawInspecter()
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

void StarFishParameter::SaveToJson()
{
	nlohmann::json paramJson = ToJson(m_param);

	std::ofstream file("Asset/Data/Enemy/StarFish/Parameter/StarFishParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("StarFish parameter save filed\n");
		KdDebugGUI::Instance().AddErrorLog("StarFish parameter save filed\n");

		return;
	}
}

void StarFishParameter::LoadFromJson()
{

	std::fstream file("Asset/Data/Enemy/StarFish/Parameter/StarFishParameter.json");

	// もしファイルを開けないとき(デフォルト値のまま動く)
	if (!file.is_open())
	{
		OutputDebugStringA("StarFishのParameter.jsonを開けませんでした\n");
		KdDebugGUI::Instance().AddErrorLog("StarFishのParameter.jsonを開けませんでした\n");
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
