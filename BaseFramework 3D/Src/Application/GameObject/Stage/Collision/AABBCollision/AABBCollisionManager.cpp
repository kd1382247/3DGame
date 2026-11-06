#include "AABBCollisionManager.h"

#include"AABBCollision.h"

#include"../../../../Scene/SceneManager.h"
#include"../../../../Editor/InspectorWidgets.h"

std::shared_ptr<AABBCollision> AABBCollisionManager::CreateAABBCollision(const std::shared_ptr<StageObject>& owner)
{
	const int id = FindAvailableID();

	std::shared_ptr<AABBCollision>aabb = std::make_shared<AABBCollision>();

	aabb->Init();

	aabb->SetID(id);

	aabb->SetOwner(owner);

	// オブジェクトの名前をセット 後ろにID
	std::string objName = "AABBCollision_" + std::to_string(id);
	aabb->SetObjectName(objName);

	SceneManager::Instance().AddObject(aabb);

	return aabb;
}

void AABBCollisionManager::RemoveByOwner(const StageObject* owner)
{
	for (const auto& aabb : GetAABBCollisionList())
	{
		if (aabb->GetOwner().get() == owner)
		{
			aabb->Destroy();
		}
	}
}

bool AABBCollisionManager::Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner)
{
	nlohmann::json rootJson;
	rootJson["AABBCollisions"] = nlohmann::json::array();

	for (const auto& aabb : GetAABBCollisionList())
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

		auto aabb = CreateAABBCollision(owner);

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
	}

	return true;
}

int AABBCollisionManager::FindAvailableID() const
{
	auto aabbList = GetAABBCollisionList();

	int id = 0;

	// 未使用のIDが見つかるまでループを回す
	while (true)
	{
		bool exists = false;

		for (const auto& aabb : aabbList)
		{
			// 今のIDがすでに使われていたらループを抜ける
			if (aabb->GetID() == id)
			{
				exists = true;
				break;
			}
		}

		// 未使用のIDが見つかったら処理終了
		if (!exists)
		{
			return id;
		}

		// IDを加算
		++id;
	}
}

std::vector<std::shared_ptr<AABBCollision>> AABBCollisionManager::GetAABBCollisionList() const
{
	std::vector<std::shared_ptr<AABBCollision>>aabbList;

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto aabb = std::dynamic_pointer_cast<AABBCollision>(obj);

		if (!aabb || aabb->IsExpired())
		{
			continue;
		}

		aabbList.push_back(aabb);
	}

	return aabbList;
}