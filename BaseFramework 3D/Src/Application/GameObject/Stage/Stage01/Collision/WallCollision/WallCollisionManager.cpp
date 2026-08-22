#include "WallCollisionManager.h"

#include"WallCollision.h"

std::shared_ptr<WallCollision> WallCollisionManager::CreateWallCollision()
{
	const int id = FindAvailableID();

	std::shared_ptr<WallCollision>wall = std::make_shared<WallCollision>();

	wall->Init();

	wall->SetID(id);

	// オブジェクトの名前をセット 後ろにID
	std::string objName = "WallCollision_" + std::to_string(id);
	wall->SetObjectName(objName);

	m_spWallCollisionList.push_back(wall);

	return wall;
}

void WallCollisionManager::RemoveWallCollision(int id)
{

	auto target = FindWallCollision(id);

	if (!target)
	{
		return;
	}

	auto it = std::find(m_spWallCollisionList.begin(), m_spWallCollisionList.end(), target);

	if (it == m_spWallCollisionList.end())
	{
		return;
	}

	m_spWallCollisionList.erase(it);

}

std::shared_ptr<WallCollision> WallCollisionManager::FindWallCollision(int id)const
{

	for (const auto& wall : m_spWallCollisionList)
	{
		if (!wall)
		{
			continue;
		}

		if (wall->GetID() == id)
		{
			return wall;
		}
	}

	return nullptr;

}

void WallCollisionManager::ClearWallCollisionList()
{
	m_spBackupList = std::move(m_spWallCollisionList);
	m_spWallCollisionList.clear();
}

void WallCollisionManager::RestoreWallCollisionList()
{
	m_spWallCollisionList = std::move(m_spBackupList);
	m_spBackupList.clear();
}

void WallCollisionManager::ClearBackup()
{
	m_spBackupList.clear();
}

bool WallCollisionManager::Save(const std::string& filePath)
{
	nlohmann::json rootJson;
	rootJson["WallCollisions"] = nlohmann::json::array();

	for (const auto& wall : m_spWallCollisionList)
	{
		if (!wall)
		{
			continue;
		}

		nlohmann::json WallCollisionJson;

		WallCollisionJson["ID"] = wall->GetID();

		WallCollisionJson["Name"] = wall->GetObjectName();

		// 座標
		const auto& pos = wall->GetPos();
		WallCollisionJson["Position"]["x"] = pos.x;
		WallCollisionJson["Position"]["y"] = pos.y;
		WallCollisionJson["Position"]["z"] = pos.z;

		// 大きさ
		const auto& scale = wall->GetScale();
		WallCollisionJson["Scale"]["x"] = scale.x;
		WallCollisionJson["Scale"]["y"] = scale.y;
		WallCollisionJson["Scale"]["z"] = scale.z;


		rootJson["WallCollisions"].push_back(WallCollisionJson);
	}

	std::ofstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA("WallCollisionDataの保存に失敗しました\n");

		return false;
	}

	file << rootJson.dump(4);

	return true;
}

bool WallCollisionManager::Load(const std::string& filePath)
{
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA(
			"WallCollisionDataを開けませんでした\n"
		);

		return false;
	}

	nlohmann::json rootJson;

	try
	{
		file >> rootJson;
	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA(
			"WallCollisionDataの読み込みに失敗しました\n"
		);

		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");

		return false;
	}

	if (!rootJson.contains("WallCollisions") ||
		!rootJson["WallCollisions"].is_array())
	{
		OutputDebugStringA(
			"WallCollisions配列がありません\n"
		);

		return false;
	}


	// 既存のWallCollisionを消す
	ClearWallCollisionList();

	// Jsonに保存されてる情報でWallCollisionを生成
	for (const auto& wallJson : rootJson["WallCollisions"])
	{

		auto obj = KdGameObjectFactory::Instance().CreateGameObject("WallCollision");

		auto wall = std::dynamic_pointer_cast<WallCollision>(obj);

		if (!wall)
		{
			continue;
		}

		wall->Init();
		// ID
		wall->SetID(wallJson["ID"].get<int>());
		// 名前
		wall->SetObjectName(wallJson["Name"].get<std::string>());
		// 座標
		wall->SetPos({
			wallJson["Position"]["x"].get<float>(),
			wallJson["Position"]["y"].get<float>(),
			wallJson["Position"]["z"].get<float>()
			});

		// 大きさ
		wall->SetScale({
			wallJson["Scale"]["x"].get<float>(),
			wallJson["Scale"]["y"].get<float>(),
			wallJson["Scale"]["z"].get<float>()
			});

		m_spWallCollisionList.push_back(wall);
	}

	return true;
}

int WallCollisionManager::FindAvailableID() const
{
	int id = 0;

	while (FindWallCollision(id))
	{
		++id;
	}

	return id;
}

void WallCollisionManager::DrawDebug()
{
	if (!IsDebug())
	{
		return;
	}

	for (const auto& wall : m_spWallCollisionList)
	{
		if (!wall){continue;}
		wall->DrawDebug();
	}
}

void WallCollisionManager::Init()
{

}
