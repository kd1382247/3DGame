#include "EnemySpawnerParameter.h"

void EnemySpawnerParameter::Init()
{
	Load();
}

void EnemySpawnerParameter::DrawInspector()
{
	if (ImGui::CollapsingHeader("Parameter", ImGuiTreeNodeFlags_DefaultOpen))
	{
		auto& parameter = m_parameter;

		if (ImGui::DragFloat("SpawnInterval", &m_parameter.m_spawnInterval), 1.0f)
		{
			m_parameter.m_spawnInterval
				=std::clamp(m_parameter.m_spawnInterval, 1.0f, 60.0f);
		}

		if (ImGui::DragInt("SpawnCount", &m_parameter.m_spawnCount), 1)
		{
			m_parameter.m_spawnCount
				= std::clamp(m_parameter.m_spawnCount, 1, 10);
		}

		if (ImGui::Button("SaveParameter"))
		{
			Save();
		}
	}
}

void EnemySpawnerParameter::Load()
{
	std::fstream file("Asset/Data/Gimmick/EnemySpawner/Parameter/EnemySpawnerParameter.json");

	if (!file.is_open())
	{
		OutputDebugStringA("ParameterData.jsonを開けませんでした\n");
		return;
	}

	nlohmann::json paramJson;

	try
	{
		file >> paramJson;
		
		m_parameter.m_spawnInterval = paramJson["SpawnInterval"].get<float>();
		m_parameter.m_spawnCount = paramJson["SpawnCount"].get<int>();

	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA("JSONの読み込みに失敗しました\n");
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		return;
	}
}

void EnemySpawnerParameter::Save()
{

	nlohmann::json paramJson;

	paramJson["SpawnInterval"] = m_parameter.m_spawnInterval;
	paramJson["SpawnCount"] = m_parameter.m_spawnCount;

	std::ofstream file("Asset/Data/Gimmick/EnemySpawner/Parameter/EnemySpawnerParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("EnemySpawner parameter save filed\n");

		return;
	}

}
