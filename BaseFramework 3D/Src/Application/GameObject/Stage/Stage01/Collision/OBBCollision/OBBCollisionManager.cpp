#include "OBBCollisionManager.h"

#include"OBBCollision.h"

std::shared_ptr<OBBCollision> OBBCollisionManager::CreateOBBCollision()
{
	const int id = FindAvailableID();

	std::shared_ptr<OBBCollision>obb = std::make_shared<OBBCollision>();

	obb->Init();

	obb->SetID(id);

	// オブジェクトの名前をセット 後ろにID
	std::string objName = "OBBCollision_" + std::to_string(id);
	obb->SetObjectName(objName);

	m_spOBBCollisionList.push_back(obb);

	return obb;
}

void OBBCollisionManager::RemoveOBBCollision(int id)
{

	auto target = FindOBBCollision(id);

	if (!target)
	{
		return;
	}

	auto it = std::find(m_spOBBCollisionList.begin(), m_spOBBCollisionList.end(), target);

	if (it == m_spOBBCollisionList.end())
	{
		return;
	}

	m_spOBBCollisionList.erase(it);

}

std::shared_ptr<OBBCollision> OBBCollisionManager::FindOBBCollision(int id)const
{

	for (const auto& obb : m_spOBBCollisionList)
	{
		if (!obb)
		{
			continue;
		}

		if (obb->GetID() == id)
		{
			return obb;
		}
	}

	return nullptr;

}

void OBBCollisionManager::ClearOBBCollisionList()
{
	m_spBackupList = std::move(m_spOBBCollisionList);
	m_spOBBCollisionList.clear();
}

void OBBCollisionManager::RestoreOBBCollisionList()
{
	m_spOBBCollisionList = std::move(m_spBackupList);
	m_spBackupList.clear();
}

void OBBCollisionManager::ClearBackup()
{
	m_spBackupList.clear();
}

bool OBBCollisionManager::Save(const std::string& filePath)
{
	nlohmann::json rootJson;
	rootJson["OBBCollisions"] = nlohmann::json::array();

	for (const auto& obb : m_spOBBCollisionList)
	{
		if (!obb)
		{
			continue;
		}

		nlohmann::json OBBCollisionJson;

		OBBCollisionJson["ID"] = obb->GetID();

		OBBCollisionJson["Name"] = obb->GetObjectName();

		// 座標
		const auto& pos = obb->GetPos();
		OBBCollisionJson["Position"]["x"] = pos.x;
		OBBCollisionJson["Position"]["y"] = pos.y;
		OBBCollisionJson["Position"]["z"] = pos.z;

		// 大きさ
		const auto& scale = obb->GetScale();
		OBBCollisionJson["Scale"]["x"] = scale.x;
		OBBCollisionJson["Scale"]["y"] = scale.y;
		OBBCollisionJson["Scale"]["z"] = scale.z;

		// 回転
		const auto& rotation = obb->GetRotation();
		OBBCollisionJson["Rotation"]["x"] = rotation.x;
		OBBCollisionJson["Rotation"]["y"] = rotation.y;
		OBBCollisionJson["Rotation"]["z"] = rotation.z;


		rootJson["OBBCollisions"].push_back(OBBCollisionJson);
	}

	std::ofstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA("OBBCollisionDataの保存に失敗しました\n");

		return false;
	}

	file << rootJson.dump(4);

	return true;
}

bool OBBCollisionManager::Load(const std::string& filePath)
{
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA(
			"OBBCollisionDataを開けませんでした\n"
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
			"OBBCollisionDataの読み込みに失敗しました\n"
		);

		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");

		return false;
	}

	if (!rootJson.contains("OBBCollisions") ||
		!rootJson["OBBCollisions"].is_array())
	{
		OutputDebugStringA(
			"OBBCollisions配列がありません\n"
		);

		return false;
	}


	// 既存のOBBCollisionを消す
	ClearOBBCollisionList();

	// Jsonに保存されてる情報でOBBCollisionを生成
	for (const auto& obbJson : rootJson["OBBCollisions"])
	{

		auto obj = KdGameObjectFactory::Instance().CreateGameObject("OBBCollision");

		auto obb = std::dynamic_pointer_cast<OBBCollision>(obj);

		if (!obb)
		{
			continue;
		}

		obb->Init();
		// ID
		obb->SetID(obbJson["ID"].get<int>());
		// 名前
		obb->SetObjectName(obbJson["Name"].get<std::string>());
		// 座標
		obb->SetPos({
			obbJson["Position"]["x"].get<float>(),
			obbJson["Position"]["y"].get<float>(),
			obbJson["Position"]["z"].get<float>()
			});

		// 大きさ
		obb->SetScale({
			obbJson["Scale"]["x"].get<float>(),
			obbJson["Scale"]["y"].get<float>(),
			obbJson["Scale"]["z"].get<float>()
			});

		// 回転
		obb->SetRotation({
			obbJson["Rotation"]["x"].get<float>(),
			obbJson["Rotation"]["y"].get<float>(),
			obbJson["Rotation"]["z"].get<float>()
			});



		m_spOBBCollisionList.push_back(obb);
	}

	return true;
}

int OBBCollisionManager::FindAvailableID() const
{
	int id = 0;

	while (FindOBBCollision(id))
	{
		++id;
	}

	return id;
}

void OBBCollisionManager::DrawDebug()
{
	if (!IsDebug())
	{
		return;
	}

	for (const auto& obb : m_spOBBCollisionList)
	{
		if (!obb) { continue; }
		obb->DrawDebug();
	}
}

void OBBCollisionManager::Init()
{

}
