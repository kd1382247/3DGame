#include "StageDataManager.h"

#include "../../Scene/SceneManager.h"
#include "../../System/WayPointManager/WayPointManager.h"
#include"../../GameObject/Stage/Stage01/Collision/WallCollision/WallCollisionManager.h"
#include"../../GameObject/Stage/Stage01/Collision/OBBCollision/OBBCollisionManager.h"


bool StageDataManager::Save(const std::string& stageName)
{
	return SaveToFolder(GetStageFolder(stageName));
}

bool StageDataManager::Load(const std::string& stageName)
{
	return LoadFromFolder(GetStageFolder(stageName));
}

bool StageDataManager::SaveTemporary()
{
	return SaveToFolder("Asset/Data/EditorTemp");
}

bool StageDataManager::LoadTemporary()
{
	return LoadFromFolder("Asset/Data/EditorTemp");
}


std::filesystem::path StageDataManager::GetStageFolder(const std::string& stageName) const
{
	return std::filesystem::path("Asset/Data/Stage") / stageName;
}

bool StageDataManager::SaveToFolder(const std::filesystem::path& folder)
{


	const std::filesystem::path stageFolder = folder;

	// フォルダーがない場合作成
	std::filesystem::create_directories(stageFolder);

	// StageDataを作成
	nlohmann::json stageJson;

	stageJson["Objects"] = nlohmann::json::array();

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		if (!obj)
		{
			continue;
		}

		if (obj->GetObjectCategory() == KdGameObject::ObjectCategory::None)
		{
			continue;
		}

		// Jsonファイルに追加
		stageJson["Objects"].push_back(obj->SaveData());
	}

	// ファイル作成
	std::ofstream file(stageFolder / "StageData.json");

	if (!file.is_open())
	{
		return false;
	}

	file << stageJson.dump(4);

	// WayPointを保存
	if (!WayPointManager::Instance().Save((stageFolder / "WayPointData.json").string()))
	{
		return false;
	}

	// Wallを保存
	if (!WallCollisionManager::Instance().Save((stageFolder / "WallCollisionData.json").string()))
	{
		return false;
	}

	// OBBを保存
	if (!OBBCollisionManager::Instance().Save((stageFolder / "OBBCollisionData.json").string()))
	{
		return false;
	}

	return true;
}

bool StageDataManager::LoadFromFolder(const std::filesystem::path& folder)
{


	const std::filesystem::path stageFolder = folder;

	// ステージデータ
	const std::filesystem::path stageDataPath = stageFolder / "StageData.json";
	// ウェイポイントデータ
	const std::filesystem::path wayPointDataPath = stageFolder / "WayPointData.json";

	// 壁の当たり判定データ
	const std::filesystem::path wallCollisionDataPath = stageFolder / "WallCollisionData.json";

	// OBBのデータ
	const std::filesystem::path obbCollisionDataPath = stageFolder / "OBBCollisionData.json";

	// 読込失敗で現在の編集内容を消さないよう、先に必要ファイルを確認する
	if (!std::filesystem::exists(stageDataPath) ||
		!std::filesystem::exists(wayPointDataPath)||
		!std::filesystem::exists(wallCollisionDataPath))
	{
		return false;
	}


	std::ifstream file(stageDataPath);

	if (!file.is_open())
	{
		return false;
	}

	nlohmann::json stageJson;
	try
	{
		file >> stageJson;
	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");

		return false;
	}

	if (!stageJson.contains("Objects") || !stageJson["Objects"].is_array())
	{
		return false;
	}

	// オブジェクトを生成
	for (const auto& objectJson : stageJson["Objects"])
	{

		if (!objectJson.contains("Class") ||
			!objectJson.contains("Name") ||
			!objectJson.contains("Position") ||
			!objectJson.contains("Scale") ||
			!objectJson.contains("Rotation"))
		{
			OutputDebugStringA("必要なデータが不足しているオブジェクトをスキップしました\n");
			continue;
		}

		const std::string className = objectJson["Class"].get<std::string>();
		auto obj = KdGameObjectFactory::Instance().CreateGameObject(className);

		if (!obj)
		{
			continue;
		}

		obj->Init();
		// 名前読込
		const std::string name = objectJson["Name"];
		obj->SetObjectName(name);

		// 座標読込
		Math::Vector3 pos;
		pos.x = objectJson["Position"]["x"].get<float>();
		pos.y = objectJson["Position"]["y"].get<float>();
		pos.z = objectJson["Position"]["z"].get<float>();

		obj->SetPos(pos);

		// 大きさ読込
		Math::Vector3 scale;
		scale.x = objectJson["Scale"]["x"].get<float>();
		scale.y = objectJson["Scale"]["y"].get<float>();
		scale.z = objectJson["Scale"]["z"].get<float>();

		obj->SetScale(scale);

		// 回転
		Math::Vector3 rotation;
		rotation.x = objectJson["Rotation"]["x"].get<float>();
		rotation.y = objectJson["Rotation"]["y"].get<float>();
		rotation.z = objectJson["Rotation"]["z"].get<float>();

		obj->SetRotation(rotation);

		SceneManager::Instance().AddObject(obj);
	}

	// ウェイポイントを生成
	if (!WayPointManager::Instance().Load(wayPointDataPath.string()))
	{
		return false;
	}

	// 壁の当たり判定を生成
	if (!WallCollisionManager::Instance().Load(wallCollisionDataPath.string()))
	{
		return false;
	}

	// OBBを生成
	if (!OBBCollisionManager::Instance().Load(obbCollisionDataPath.string()))
	{
		return false;
	}

	return true;
}
