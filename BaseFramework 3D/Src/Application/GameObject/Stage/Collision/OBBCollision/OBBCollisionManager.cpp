#include "OBBCollisionManager.h"

#include"OBBCollision.h"

#include"../../../../Scene/SceneManager.h"
#include"../../../../Editor/InspectorWidgets.h"

std::shared_ptr<OBBCollision> OBBCollisionManager::CreateOBBCollision(const std::shared_ptr<StageObject>& owner)
{
	const int id = FindAvailableID();

	std::shared_ptr<OBBCollision>obb = std::make_shared<OBBCollision>();

	obb->Init();

	obb->SetID(id);

	obb->SetParent(owner);

	// オブジェクトの名前をセット 後ろにID
	std::string objName = "OBBCollision_" + std::to_string(id);
	obb->SetObjectName(objName);

	SceneManager::Instance().AddObject(obb);

	return obb;
}

void OBBCollisionManager::RemoveByOwner(const StageObject* owner)
{

	for (const auto& obb : GetOBBCollisionList())
	{
		if (obb->GetParent().get() == owner)
		{
			obb->Destroy();
		}
	}

}

bool OBBCollisionManager::Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner)
{
	nlohmann::json rootJson;
	rootJson["OBBCollisions"] = nlohmann::json::array();

	for (const auto& obb : GetOBBCollisionList())
	{
		if (!obb ||
			obb->GetParent() != owner)
		{
			continue;
		}


		nlohmann::json OBBCollisionJson;

		OBBCollisionJson["ID"] = obb->GetID();

		OBBCollisionJson["Name"] = obb->GetObjectName();

		// 座標(Stageからのローカル位置)
		const auto& pos = obb->GetLocalPos();
		OBBCollisionJson["Position"]["x"] = InspectorUI::CeilToDigits(pos.x, 3);
		OBBCollisionJson["Position"]["y"] = InspectorUI::CeilToDigits(pos.y, 3);
		OBBCollisionJson["Position"]["z"] = InspectorUI::CeilToDigits(pos.z, 3);

		// 大きさ(Stageからのローカル大きさ)
		const auto& scale = obb->GetLocalScale();
		OBBCollisionJson["Scale"]["x"] = InspectorUI::CeilToDigits(scale.x, 3);
		OBBCollisionJson["Scale"]["y"] = InspectorUI::CeilToDigits(scale.y, 3);
		OBBCollisionJson["Scale"]["z"] = InspectorUI::CeilToDigits(scale.z, 3);

		// 回転(Stageには追従しない、OBB自身の回転)
		const auto& rotation = obb->GetRotation();
		OBBCollisionJson["Rotation"]["x"] = InspectorUI::CeilToDigits(rotation.x, 3);
		OBBCollisionJson["Rotation"]["y"] = InspectorUI::CeilToDigits(rotation.y, 3);
		OBBCollisionJson["Rotation"]["z"] = InspectorUI::CeilToDigits(rotation.z, 3);

		const auto& collitionTypeName = obb->GetCollisionTypeName();
		OBBCollisionJson["CollisionType"] = collitionTypeName;

		rootJson["OBBCollisions"].push_back(OBBCollisionJson);
	}

	std::ofstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA("OBBCollisionDataの保存に失敗しました\n");
		KdDebugGUI::Instance().AddErrorLog("OBBCollisionDataの保存に失敗しました\n");

		return false;
	}

	file << rootJson.dump(4);

	return true;
}

bool OBBCollisionManager::Load(const std::string& filePath, const std::shared_ptr<StageObject>& owner)
{
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA(
			"OBBCollisionDataを開けませんでした\n"
		);
		KdDebugGUI::Instance().AddErrorLog("OBBCollisionDataを開けませんでした\n");

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
		KdDebugGUI::Instance().AddErrorLog("OBBCollisionDataの読み込みに失敗しました\n");

		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		KdDebugGUI::Instance().AddErrorLog("%s\n", e.what());

		return false;
	}

	if (!rootJson.contains("OBBCollisions") ||
		!rootJson["OBBCollisions"].is_array())
	{
		OutputDebugStringA(
			"OBBCollisions配列がありません\n"
		);
		KdDebugGUI::Instance().AddErrorLog("OBBCollisions配列がありません\n");

		return false;
	}

	// Jsonに保存されてる情報でOBBCollisionを生成
	for (const auto& obbJson : rootJson["OBBCollisions"])
	{
		auto obb = CreateOBBCollision(owner);

		// 座標(Stageからのローカル位置)
		obb->SetLocalPos({
			obbJson["Position"]["x"].get<float>(),
			obbJson["Position"]["y"].get<float>(),
			obbJson["Position"]["z"].get<float>()
			});

		// 大きさ(Stageからのローカル大きさ)
		obb->SetLocalScale({
			obbJson["Scale"]["x"].get<float>(),
			obbJson["Scale"]["y"].get<float>(),
			obbJson["Scale"]["z"].get<float>()
			});

		// 回転(Stageには追従しない、OBB自身の回転)
		obb->SetRotation({
			obbJson["Rotation"]["x"].get<float>(),
			obbJson["Rotation"]["y"].get<float>(),
			obbJson["Rotation"]["z"].get<float>()
			});


		// 当たり判定タイプ
		if (obbJson.contains("CollisionType"))
		{
			obb->SetCollisionType(obbJson["CollisionType"].get<std::string>());
		}
	}

	return true;
}

int OBBCollisionManager::FindAvailableID() const
{
	auto obbList = GetOBBCollisionList();

	int id = 0;

	// 未使用のIDが見つかるまでループを回す
	while (true)
	{
		bool exists = false;

		for (const auto& obb : obbList)
		{
			// 今のIDがすでに使われていたらループを抜ける
			if (obb->GetID() == id)
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

std::vector<std::shared_ptr<OBBCollision>> OBBCollisionManager::GetOBBCollisionList() const
{
	std::vector<std::shared_ptr<OBBCollision>>obbList;

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto obb = std::dynamic_pointer_cast<OBBCollision>(obj);

		if (!obb || obb->IsExpired())
		{
			continue;
		}

		obbList.push_back(obb);
	}

	return obbList;
}
