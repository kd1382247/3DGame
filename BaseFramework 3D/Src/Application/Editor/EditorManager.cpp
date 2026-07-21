#include "EditorManager.h"

#include"../GameObject/Character/Player/Player.h"
#include"../GameObject/Terrains/Ground/Ground.h"
#include"../GameObject/Camera/TPSCamera/TPSCamera.h"

#include"json.hpp"
#include<fstream>


#include"../Scene/SceneManager.h"

void EditorManager::Init()
{
	m_gameObjectFactory.RegisterCreateFunction(
		"KdGameObject",[](){return std::make_shared<KdGameObject>();});

	// カメラ
	m_gameObjectFactory.RegisterCreateFunction(
		"TPSCamera", []() {return std::make_shared<TPSCamera>(); }
	);

	// プレイヤー
	m_gameObjectFactory.RegisterCreateFunction(
		"Player", []() {return std::make_shared<Player>(); }
	);

	// 地面
	m_gameObjectFactory.RegisterCreateFunction(
		"Ground", []() {return std::make_shared<Ground>(); }
	);

}

void EditorManager::Draw()
{

	DrawMenu();

	if (ImGui::Button("Save Scene"))
	{
		SaveScene();
	}

	ImGui::SameLine();

	if (ImGui::Button("Load Scene"))
	{
		LoadScene();
	}


	//m_hierarchy.Draw();
	//m_inspector.Draw();
}

void EditorManager::DrawMenu()
{
	if(ImGui::Button("Edit"))
	{
		SetMode(EditorMode::Edit);
	}

	ImGui::SameLine();

	if (ImGui::Button("Play"))
	{
		SetMode(EditorMode::Play);
	}

	if (IsEditMode())
	{
		ImGui::Text("CurrentMode: Edit");
	}
	else
	{
		ImGui::Text("CurrentMode: Play");
	}
}

void EditorManager::SaveScene()
{

	nlohmann::json sceneJson;

	// 配列作成
	sceneJson["Objects"] = nlohmann::json::array();
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		// 各オブジェクトをセーブし配列に格納
		sceneJson["Objects"].push_back(obj->SaveData());
	}

	// デバッグに保存先のファイルパスを表示
	std::filesystem::path path = std::filesystem::current_path();
	OutputDebugStringA((path.string() + "\n").c_str());

	// ファイルに保存
	std::ofstream file("SceneData.json");
	if (file.is_open())
	{
		// JSONを文字列に変換()はインデント幅
		file << sceneJson.dump(4);
		file.close();

		OutputDebugStringA("Scene save success\n");
	}
	else
	{
		OutputDebugStringA("Scene save failed\n");
	}


}

void EditorManager::LoadScene()
{
	std::ifstream file("SceneData.json");

	// もしファイルを開けないとき
	if (!file.is_open())
	{
		OutputDebugStringA("SceneData.jsonを開けませんでした\n");
		return;
	}

	nlohmann::json sceneJson;

	try
	{
		file >> sceneJson;
	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA("JSONの読み込みに失敗しました\n");
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		return;
	}

	// 現在選択しているオブジェクトへの参照を解除
	SetSelectedObject(nullptr);

	// 現在のオブジェクトを削除予約
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		if (obj)
		{
			obj->Destroy();
		}
	}

	if (!sceneJson.contains("Objects") ||
		!sceneJson["Objects"].is_array())
	{
		OutputDebugStringA("Objects配列がありません\n");
		return;
	}

	for (const auto& objectJson : sceneJson["Objects"])
	{

		if (!objectJson.contains("Class") ||
			!objectJson.contains("Name") ||
			!objectJson.contains("Position") ||
			!objectJson.contains("Scale"))
		{
			OutputDebugStringA("必要なデータが不足しているオブジェクトをスキップしました\n");
			continue;
		}

		std::string className = objectJson["Class"].get<std::string>();
		auto obj = m_gameObjectFactory.CreateGameObject(className);

		if (!obj)
		{
			continue;
		}

		obj->Init();
		// 名前読込
		std::string name = objectJson["Name"];
		obj->SetObjectName(name);

		// 座標読込
		Math::Vector3 pos;
		pos.x = objectJson["Position"]["x"].get<float>();;
		pos.y = objectJson["Position"]["y"].get<float>();;
		pos.z = objectJson["Position"]["z"].get<float>();;

		obj->SetPos(pos);

		// 大きさ読込
		Math::Vector3 scale;
		scale.x = objectJson["Scale"]["x"].get<float>();;
		scale.y = objectJson["Scale"]["y"].get<float>();;
		scale.z = objectJson["Scale"]["z"].get<float>();;

		obj->SetScale(scale);

		SceneManager::Instance().AddObject(obj);
	}

	// ③ 全部生成した後に探す
	std::shared_ptr<Player> player;
	std::shared_ptr<TPSCamera> camera;
	std::shared_ptr<Ground>ground;
	for (const auto& obj :
		SceneManager::Instance().GetObjList())
	{
		if (obj->GetObjectName()=="Player")
		{
			player =
				std::dynamic_pointer_cast<Player>(obj);
		}

		if (obj->GetObjectName() == "TPSCamera")
		{
			camera =
				std::dynamic_pointer_cast<TPSCamera>(obj);
		}
		if (obj->GetObjectName() == "Ground")
		{
			ground =
				std::dynamic_pointer_cast<Ground>(obj);
		}
	}

	// ④ 関連付ける
	if (player && camera)
	{
		camera->SetTarget(player);
	}

	if (player && ground)
	{
		player->RegistHitObject(ground);
	}

}
