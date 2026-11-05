#include "AABBCollisionManager.h"

#include"AABBCollision.h"

#include"../../../../Editor/EditorManager.h"
#include"../../../../Editor/InspectorWidgets.h"

std::shared_ptr<AABBCollision> AABBCollisionManager::CreateAABBCollision()
{
	const int id = FindAvailableID();

	std::shared_ptr<AABBCollision>aabb = std::make_shared<AABBCollision>();

	aabb->Init();

	aabb->SetID(id);

	// オブジェクトの名前をセット 後ろにID
	std::string objName = "AABBCollision_" + std::to_string(id);
	aabb->SetObjectName(objName);

	m_spAABBCollisionList.push_back(aabb);

	return aabb;
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

void AABBCollisionManager::RemoveByOwner(const StageObject* owner)
{
	std::erase_if(m_spAABBCollisionList,
		[owner](const std::shared_ptr<AABBCollision>& aabb)
		{
			return aabb&&aabb->GetOwner().get() == owner;
		}
	);
}

std::shared_ptr<AABBCollision> AABBCollisionManager::FindAABBCollision(int id)const
{

	for (const auto& aabb : m_spAABBCollisionList)
	{
		if (!aabb)
		{
			continue;
		}

		if (aabb->GetID() == id)
		{
			return aabb;
		}
	}

	return nullptr;

}

void AABBCollisionManager::Backup()
{
	m_spBackupList = std::move(m_spAABBCollisionList);
	m_spAABBCollisionList.clear();
}

void AABBCollisionManager::Restore()
{
	m_spAABBCollisionList = std::move(m_spBackupList);
	m_spBackupList.clear();
}

void AABBCollisionManager::Discard()
{
	m_spBackupList.clear();
}

bool AABBCollisionManager::Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner)
{
	nlohmann::json rootJson;
	rootJson["AABBCollisions"] = nlohmann::json::array();

	for (const auto& aabb : m_spAABBCollisionList)
	{
		if (!aabb||
			aabb->GetOwner() != owner)
		{
			continue;
		}

		nlohmann::json AABBCollisionJson;

		AABBCollisionJson["ID"] = aabb->GetID();

		AABBCollisionJson["Name"] = aabb->GetObjectName();

		// 座標(Stageからのローカル位置)
		const auto& pos = aabb->GetLocalPos();
		AABBCollisionJson["Position"]["x"] = InspectorUI::CeilToDigits(pos.x, 3);
		AABBCollisionJson["Position"]["y"] = InspectorUI::CeilToDigits(pos.y, 3);
		AABBCollisionJson["Position"]["z"] = InspectorUI::CeilToDigits(pos.z, 3);

		// 大きさ(Stageからのローカル大きさ)
		const auto& scale = aabb->GetLocalScale();
		AABBCollisionJson["Scale"]["x"] = InspectorUI::CeilToDigits(scale.x, 3);
		AABBCollisionJson["Scale"]["y"] = InspectorUI::CeilToDigits(scale.y, 3);
		AABBCollisionJson["Scale"]["z"] = InspectorUI::CeilToDigits(scale.z, 3);


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

bool AABBCollisionManager::Load(const std::string& filePath, const std::shared_ptr<StageObject>& owner)
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


	// Jsonに保存されてる情報でAABBCollisionを生成
	for (const auto& aabbJson : rootJson["AABBCollisions"])
	{

		auto obj = KdGameObjectFactory::Instance().CreateGameObject("AABBCollision");

		auto aabb = std::dynamic_pointer_cast<AABBCollision>(obj);

		if (!aabb)
		{
			continue;
		}

		aabb->Init();
		// ID
		aabb->SetID(FindAvailableID());
		// 名前
		aabb->SetObjectName("AABBCollision_" + std::to_string(aabb->GetID()));
		// 座標(Stageからのローカル位置)
		aabb->SetLocalPos({
			aabbJson["Position"]["x"].get<float>(),
			aabbJson["Position"]["y"].get<float>(),
			aabbJson["Position"]["z"].get<float>()
			});

		// 大きさ(Stageからのローカル大きさ)
		aabb->SetLocalScale({
			aabbJson["Scale"]["x"].get<float>(),
			aabbJson["Scale"]["y"].get<float>(),
			aabbJson["Scale"]["z"].get<float>()
			});

		aabb->SetOwner(owner);

		m_spAABBCollisionList.push_back(aabb);
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

	for (const auto& aabb : m_spAABBCollisionList)
	{
		if (!aabb){continue;}
		// 持ち主が違う場合表示しない
		if (aabb->GetOwner() != EditorManager::Instance().GetActiveStage())
		{
			continue;
		}

		aabb->DrawDebug();
	}
}

void AABBCollisionManager::SetStageTransform(const StageObject* stageObject, const Math::Vector3& stagePos, const Math::Vector3& stageScale)
{
	for (const auto& aabb : m_spAABBCollisionList)
	{
		if (!aabb ||
			!aabb->ShouldFollow(stageObject))
		{
			continue;
		}


		aabb->SetStageTransform(stagePos, stageScale);
	}
}

void AABBCollisionManager::Init()
{

}
