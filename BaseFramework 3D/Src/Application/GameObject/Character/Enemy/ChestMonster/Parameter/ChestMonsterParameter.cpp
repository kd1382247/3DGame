#include "ChestMonsterParameter.h"

#include"../../../../../Editor/EditorManager.h"
#include"../../../../../Editor/InspectorWidgets.h"
#include"../../../ParameterJson.h"
#include"../../SpawnableEnemies.h"

#include"json.hpp"
#include<fstream>


namespace
{
	// Parameter → json
	nlohmann::json ParamToJson(const ChestMonsterParameter::Parameter& param)
	{
		nlohmann::json json;

		json["MaxHP"] = param.m_maxHP;
		json["BumpSphereRadius"] = param.m_bumpSphereRadius;
		json["BumpSphereOffsetY"] = param.m_bumpSphereOffsetY;
		json["Scale"] = param.m_scale;
		json["MoveSpeed"] = param.m_moveSpeed;
		json["TurnSpeed"] = param.m_turnSpeed;
		json["ReachDistance"] = param.m_reachDistance;
		json["JumpPower"] = param.m_jumpPow;
		json["AttackPower"] = param.m_attackPow;
		json["SpawnInterval"] = param.m_spawnInterval;
		json["SpawnStartFrame"] = param.m_spawnStartFrame;
		json["SpawnEndFrame"] = param.m_spawnEndFrame;
		json["SpawnWaitFrame"] = param.m_spawnWaitFrame;
		json["MaxSpawnedAlive"] = param.m_maxSpawnedAlive;
		json["MaxTotalEnemies"] = param.m_maxTotalEnemies;
		json["SpawnEnemies"] = param.m_spawnEnemyNames;
		json["SpawnHeight"] = param.m_spawnHeight;
		json["SpawnLaunchPower"] = param.m_spawnLaunchPower;
		json["SpawnLaunchSpeed"] = param.m_spawnLaunchSpeed;

		return json;
	}

	// json → Parameter(キーが無い項目は、デフォルト値のまま)
	void ParamFromJson(const nlohmann::json& json, ChestMonsterParameter::Parameter& param)
	{
		ParameterJson::Read(json, "MaxHP", param.m_maxHP);
		ParameterJson::Read(json, "BumpSphereRadius", param.m_bumpSphereRadius);
		ParameterJson::Read(json, "BumpSphereOffsetY", param.m_bumpSphereOffsetY);
		ParameterJson::Read(json, "Scale", param.m_scale);
		ParameterJson::Read(json, "MoveSpeed", param.m_moveSpeed);
		ParameterJson::Read(json, "TurnSpeed", param.m_turnSpeed);
		ParameterJson::Read(json, "ReachDistance", param.m_reachDistance);
		ParameterJson::Read(json, "JumpPower", param.m_jumpPow);
		ParameterJson::Read(json, "AttackPower", param.m_attackPow);
		ParameterJson::Read(json, "SpawnInterval", param.m_spawnInterval);
		ParameterJson::Read(json, "SpawnStartFrame", param.m_spawnStartFrame);
		ParameterJson::Read(json, "SpawnEndFrame", param.m_spawnEndFrame);
		ParameterJson::Read(json, "SpawnWaitFrame", param.m_spawnWaitFrame);
		ParameterJson::Read(json, "MaxSpawnedAlive", param.m_maxSpawnedAlive);
		ParameterJson::Read(json, "MaxTotalEnemies", param.m_maxTotalEnemies);

		// 召喚する敵の名前(キーが無いときは、デフォルトのまま)
		if (json.is_object() && json.contains("SpawnEnemies") && json["SpawnEnemies"].is_array())
		{
			param.m_spawnEnemyNames.clear();

			for (const auto& enemyName : json["SpawnEnemies"])
			{
				if (enemyName.is_string())
				{
					param.m_spawnEnemyNames.emplace(enemyName.get<std::string>());
				}
			}
		}

		ParameterJson::Read(json, "SpawnHeight", param.m_spawnHeight);
		ParameterJson::Read(json, "SpawnLaunchPower", param.m_spawnLaunchPower);
		ParameterJson::Read(json, "SpawnLaunchSpeed", param.m_spawnLaunchSpeed);
	}
}

void ChestMonsterParameter::Init()
{
	LoadFromJson();
}

namespace
{
	// 1つ分のパラメータをグループごとに描画する
	void DrawParameter_(ChestMonsterParameter::Parameter& param)
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
			InspectorUI::Power("JumpPower", param.m_jumpPow);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Attack"))
		{
			InspectorUI::Damage("AttackPower", param.m_attackPow);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Spawn"))
		{
			InspectorUI::Seconds("Interval", param.m_spawnInterval);
			InspectorUI::Frame("StartFrame", param.m_spawnStartFrame);
			InspectorUI::Frame("EndFrame", param.m_spawnEndFrame);
			InspectorUI::Frame("WaitFrame", param.m_spawnWaitFrame);
			InspectorUI::Count("MaxAlive", param.m_maxSpawnedAlive, 1);
			InspectorUI::Count("MaxTotal", param.m_maxTotalEnemies, 1);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Spawned Enemy"))
		{
			// 召喚する敵の種類(チェックした敵の中からランダムに選ばれる)
			ImGui::TextUnformatted("Enemies");

			for (const auto& enemyName : SpawnableEnemies::Names)
			{
				bool checked = param.m_spawnEnemyNames.contains(enemyName);

				if (ImGui::Checkbox(enemyName.c_str(), &checked))
				{
					if (checked)
					{
						param.m_spawnEnemyNames.insert(enemyName);
					}
					else
					{
						param.m_spawnEnemyNames.erase(enemyName);
					}

					EditorManager::Instance().MarkDirty();
				}
			}

			InspectorUI::Meter("Height", param.m_spawnHeight);
			InspectorUI::Power("LaunchPower", param.m_spawnLaunchPower);
			InspectorUI::MeterPerFrame("LaunchSpeed", param.m_spawnLaunchSpeed);

			InspectorUI::EndGroup();
		}
	}
}

void ChestMonsterParameter::DrawInspecter()
{
	if (ImGui::CollapsingHeader("Parameter", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 単位の説明(m / s / F / ° など)
		InspectorUI::UnitLegend();

		// セーブ
		if (ImGui::Button("SaveAsDefault"))
		{
			SaveToJson();
		}

		DrawParameter_(m_param);
	}
}

nlohmann::json ChestMonsterParameter::ToJson() const
{
	return ParamToJson(m_param);
}

void ChestMonsterParameter::FromJson(const nlohmann::json& json)
{
	ParamFromJson(json, m_param);
}

void ChestMonsterParameter::SaveToJson()
{
	nlohmann::json paramJson = ParamToJson(m_param);

	std::ofstream file("Asset/Data/Enemy/ChestMonster/Parameter/ChestMonsterParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("ChestMonster parameter save filed\n");
		KdDebugGUI::Instance().AddErrorLog("ChestMonster parameter save filed\n");

		return;
	}
}

void ChestMonsterParameter::LoadFromJson()
{

	std::fstream file("Asset/Data/Enemy/ChestMonster/Parameter/ChestMonsterParameter.json");

	// もしファイルを開けないとき(デフォルト値のまま動く)
	if (!file.is_open())
	{
		OutputDebugStringA("ChestMonsterのParameter.jsonを開けませんでした\n");
		KdDebugGUI::Instance().AddErrorLog("ChestMonsterのParameter.jsonを開けませんでした\n");
		return;
	}

	nlohmann::json paramJson;

	try
	{
		file >> paramJson;

		ParamFromJson(paramJson, m_param);
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
