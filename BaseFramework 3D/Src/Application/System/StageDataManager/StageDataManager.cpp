#include "StageDataManager.h"

#include"../../GameObject/Stage/StageObject.h"

#include "../../Scene/SceneManager.h"

#include"../../GameObject/WayPoint/WayPoint.h"
#include"../../GameObject/Stage/Collision/AABBCollision/AABBCollision.h"
#include"../../GameObject/Stage/Collision/OBBCollision/OBBCollision.h"

#include "../../System/WayPointManager/WayPointManager.h"
#include"../../GameObject/Stage/Collision/AABBCollision/AABBCollisionManager.h"
#include"../../GameObject/Stage/Collision/OBBCollision/OBBCollisionManager.h"
#include"../../System/StageLoder/StageLoder.h"


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

bool StageDataManager::LoadStageModelData(const std::shared_ptr<StageObject>& stage)
{
	if (!stage)
	{
		return false;
	}

	const std::string modelName = stage->GetStageModelName();

	if (modelName.empty())
	{
		return true;
	}

	// モデル名に対応する各データのファイルパスを取得
	const std::filesystem::path aabbFilePath =
		StageLoder::Instance().GetAABBCollisionDataPath(modelName);
	const std::filesystem::path obbFilePath =
		StageLoder::Instance().GetOBBCollisionDataPath(modelName);
	const std::filesystem::path wayPointFilePath =
		StageLoder::Instance().GetWayPointDataPath(modelName);

	if (std::filesystem::exists(aabbFilePath))
	{
		if (!AABBCollisionManager::Instance().Load(aabbFilePath.string(), stage))
		{
			return false;
		}
	}

	if (std::filesystem::exists(obbFilePath))
	{
		if (!OBBCollisionManager::Instance().Load(obbFilePath.string(), stage))
		{
			return false;
		}
	}

	if (std::filesystem::exists(wayPointFilePath))
	{
		if (!WayPointManager::Instance().Load(wayPointFilePath.string(), stage))
		{
			return false;
		}
	}

	return true;

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
	{
		std::ofstream file(stageFolder / "StageData.json");

		if (!file.is_open())
		{
			return false;
		}

		file << stageJson.dump(4);
	}

	// シーン内のStageObjectを集める
	std::vector<std::shared_ptr<StageObject>> stageObjects;

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto stageObj = std::dynamic_pointer_cast<StageObject>(obj);

		if (stageObj)
		{
			stageObjects.push_back(stageObj);
		}
	}

	// 持ち主のいないデータを、先頭のStageObjectに引き取らせる
	// (エディタで作成時に持ち主を付けるようになるまでのつなぎ)
	if (!stageObjects.empty())
	{
		const auto& firstStage = stageObjects.front();

		for (const auto& wayPoint : WayPointManager::Instance().GetWayPoints())
		{
			if (wayPoint && !wayPoint->GetOwner())
			{
				wayPoint->SetOwner(firstStage);
			}
		}

		for (const auto& aabb : AABBCollisionManager::Instance().GetAABBCollisionList())
		{
			if (aabb && !aabb->GetOwner())
			{
				aabb->SetOwner(firstStage);
			}
		}

		for (const auto& obb : OBBCollisionManager::Instance().GetOBBCollisionList())
		{
			if (obb && !obb->GetOwner())
			{
				obb->SetOwner(firstStage);
			}
		}
	}

	// 保存済みのモデル名(同じモデルを2回保存しないため)
	std::unordered_set<std::string> savedModelNames;

	for (const auto& stage : stageObjects)
	{
		const std::string modelName = stage->GetStageModelName();

		if (modelName.empty())
		{
			continue;
		}

		// 同じモデルが複数ある場合は、最初の1つだけ保存する
		// (エディタで編集できるのも最初の1つだけなので、これは正常な動作)
		if (!savedModelNames.insert(modelName).second)
		{
			continue;
		}

		// AABB/OBB/WayPointは、ステージモデル単位のフォルダーに保存する
		const std::filesystem::path modelDataFolder =
			StageLoder::Instance().GetAABBCollisionDataPath(modelName).parent_path();

		// フォルダーがない場合作成
		std::filesystem::create_directories(modelDataFolder);

		// WayPointを保存
		if (!WayPointManager::Instance().Save(
			StageLoder::Instance().GetWayPointDataPath(modelName).string(), stage))
		{
			return false;
		}

		// AABBを保存
		if (!AABBCollisionManager::Instance().Save(
			StageLoder::Instance().GetAABBCollisionDataPath(modelName).string(), stage))
		{
			return false;
		}

		// OBBを保存
		if (!OBBCollisionManager::Instance().Save(
			StageLoder::Instance().GetOBBCollisionDataPath(modelName).string(), stage))
		{
			return false;
		}
	}

	return true;
}

bool StageDataManager::LoadFromFolder(const std::filesystem::path& folder)
{


	const std::filesystem::path stageFolder = folder;

	// ステージデータ
	const std::filesystem::path stageDataPath = stageFolder / "StageData.json";

	// 読込失敗で現在の編集内容を消さないよう、先に必要ファイルを確認する
	if (!std::filesystem::exists(stageDataPath))
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
		KdDebugGUI::Instance().AddErrorLog("%s\n", e.what());

		return false;
	}

	if (!stageJson.contains("Objects") || !stageJson["Objects"].is_array())
	{
		return false;
	}


	// ステージのリスト
	std::vector<std::shared_ptr<StageObject>>StageObjects;

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
			KdDebugGUI::Instance().AddErrorLog("必要なデータが不足しているオブジェクトをスキップしました\n");
			continue;
		}

		const std::string className = objectJson["Class"].get<std::string>();
		auto obj = KdGameObjectFactory::Instance().CreateGameObject(className);

		if (!obj)
		{
			continue;
		}

		obj->Init();

		// クラス名を保持(再度SaveDataする時に"Class"が壊れないようにする)
		obj->SetFactoryClassName(className);

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

		// そのクラス固有の追加データを読み込む(何もオーバーライドしていなければ何もしない)
		obj->LoadData(objectJson);

		// 読み込まれたクラスがステージの場合は
		// キャストしてステージリストに追加
		if (auto stageObj=std::dynamic_pointer_cast<StageObject>(obj))
		{
			StageObjects.push_back(stageObj);
		}

		SceneManager::Instance().AddObject(obj);
	}

	// 各ステージに対応する当たり判定、ウェイポイントをロード
	// ロードした際に持ち主(ステージ)を渡す
	for (const auto& stage : StageObjects)
	{
		if (!LoadStageModelData(stage))
		{
			return false;
		}
	}

	return true;
}
