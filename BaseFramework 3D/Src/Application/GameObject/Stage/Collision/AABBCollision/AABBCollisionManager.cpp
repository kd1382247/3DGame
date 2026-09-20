#include "AABBCollisionManager.h"

#include"AABBCollision.h"

std::shared_ptr<AABBCollision> AABBCollisionManager::CreateAABBCollision()
{
	const int id = FindAvailableID();

	std::shared_ptr<AABBCollision>wall = std::make_shared<AABBCollision>();

	wall->Init();

	wall->SetID(id);

	// オブジェクトの名前をセット 後ろにID
	std::string objName = "AABBCollision_" + std::to_string(id);
	wall->SetObjectName(objName);

	m_spAABBCollisionList.push_back(wall);

	return wall;
}

void AABBCollisionManager::RemoveAABBCollision(int id)
{

	auto target = FindAABBCollision(id);

	if (!target)
	{
		return;
	}

	auto it = std::find(m_spAABBCollisionList.begin(), m_spAABBCollisionList.end(), target);

	if (it == m_spAABBCollisionList.end())
	{
		return;
	}

	m_spAABBCollisionList.erase(it);

}

std::shared_ptr<AABBCollision> AABBCollisionManager::FindAABBCollision(int id)const
{

	for (const auto& wall : m_spAABBCollisionList)
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

void AABBCollisionManager::ClearAABBCollisionList()
{
	m_spBackupList = std::move(m_spAABBCollisionList);
	m_spAABBCollisionList.clear();
}

void AABBCollisionManager::RestoreAABBCollisionList()
{
	m_spAABBCollisionList = std::move(m_spBackupList);
	m_spBackupList.clear();
}

void AABBCollisionManager::ClearBackup()
{
	m_spBackupList.clear();
}

bool AABBCollisionManager::Save(const std::string& filePath)
{
	nlohmann::json rootJson;
	rootJson["AABBCollisions"] = nlohmann::json::array();

	for (const auto& wall : m_spAABBCollisionList)
	{
		if (!wall)
		{
			continue;
		}

		nlohmann::json AABBCollisionJson;

		AABBCollisionJson["ID"] = wall->GetID();

		AABBCollisionJson["Name"] = wall->GetObjectName();

		// 座標(Stageからのローカル位置)
		const auto& pos = wall->GetLocalPos();
		AABBCollisionJson["Position"]["x"] = pos.x;
		AABBCollisionJson["Position"]["y"] = pos.y;
		AABBCollisionJson["Position"]["z"] = pos.z;

		// 大きさ(Stageからのローカル大きさ)
		const auto& scale = wall->GetLocalScale();
		AABBCollisionJson["Scale"]["x"] = scale.x;
		AABBCollisionJson["Scale"]["y"] = scale.y;
		AABBCollisionJson["Scale"]["z"] = scale.z;


		rootJson["AABBCollisions"].push_back(AABBCollisionJson);
	}

	std::ofstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA("AABBCollisionDataの保存に失敗しました\n");
		KdDebugGUI::Instance().AddErrorLog("AABBCollisionDataの保存に失敗しました\n");

		return false;
	}

	file << rootJson.dump(4);

	return true;
}

bool AABBCollisionManager::Load(const std::string& filePath)
{
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA(
			"AABBCollisionDataを開けませんでした\n"
		);
		KdDebugGUI::Instance().AddErrorLog("AABBCollisionDataを開けませんでした\n");

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
			"AABBCollisionDataの読み込みに失敗しました\n"
		);
		KdDebugGUI::Instance().AddErrorLog("AABBCollisionDataの読み込みに失敗しました\n");

		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		KdDebugGUI::Instance().AddErrorLog("%s\n", e.what());

		return false;
	}

	if (!rootJson.contains("AABBCollisions") ||
		!rootJson["AABBCollisions"].is_array())
	{
		OutputDebugStringA(
			"AABBCollisions配列がありません\n"
		);
		KdDebugGUI::Instance().AddErrorLog("AABBCollisions配列がありません\n");

		return false;
	}


	// 既存のAABBCollisionを消す
	ClearAABBCollisionList();

	// Jsonに保存されてる情報でAABBCollisionを生成
	for (const auto& wallJson : rootJson["AABBCollisions"])
	{

		auto obj = KdGameObjectFactory::Instance().CreateGameObject("AABBCollision");

		auto wall = std::dynamic_pointer_cast<AABBCollision>(obj);

		if (!wall)
		{
			continue;
		}

		wall->Init();
		// ID
		wall->SetID(wallJson["ID"].get<int>());
		// 名前
		wall->SetObjectName(wallJson["Name"].get<std::string>());
		// 座標(Stageからのローカル位置)
		wall->SetLocalPos({
			wallJson["Position"]["x"].get<float>(),
			wallJson["Position"]["y"].get<float>(),
			wallJson["Position"]["z"].get<float>()
			});

		// 大きさ(Stageからのローカル大きさ)
		wall->SetLocalScale({
			wallJson["Scale"]["x"].get<float>(),
			wallJson["Scale"]["y"].get<float>(),
			wallJson["Scale"]["z"].get<float>()
			});

		m_spAABBCollisionList.push_back(wall);
	}

	return true;
}

int AABBCollisionManager::FindAvailableID() const
{
	int id = 0;

	while (FindAABBCollision(id))
	{
		++id;
	}

	return id;
}

void AABBCollisionManager::DrawDebug()
{
	if (!IsDebug())
	{
		return;
	}

	for (const auto& wall : m_spAABBCollisionList)
	{
		if (!wall){continue;}
		wall->DrawDebug();
	}
}

void AABBCollisionManager::SetStageTransform(const Math::Vector3& stagePos, const Math::Vector3& stageScale)
{
	for (const auto& wall : m_spAABBCollisionList)
	{
		if (!wall)
		{
			continue;
		}

		wall->SetStageTransform(stagePos, stageScale);
	}
}

void AABBCollisionManager::Init()
{

}
