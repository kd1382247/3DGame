#include "EditorManager.h"

#include"json.hpp"

#include"../../Framework/GameObject/KdGameObjectFactory.h"


#include"../Scene/SceneManager.h"

void EditorManager::Init()
{

}

void EditorManager::Draw()
{
	ImGui::Begin("Menu");

	m_stageEditor.Draw();

	DrawMenu();

	ImGui::End();

	m_hierarchy.Draw();
	m_inspector.Draw();

}

void EditorManager::DrawMenu()
{


	if (IsEditMode())
	{
		ImGui::Text("CurrentMode: Edit");
	}
	else
	{
		ImGui::Text("CurrentMode: Play");
	}

	if(ImGui::Button("Edit"))
	{
		SetEditorMode(EditorMode::Edit);
	}

	ImGui::SameLine();

	if (ImGui::Button("Play"))
	{
		SetEditorMode(EditorMode::Play);
	}

}

void EditorManager::SetupObjectReferences()
{
	
}

void EditorManager::CreateGameObject(const std::string& className)
{
	auto newObject =
		KdGameObjectFactory::Instance().CreateGameObject(className);

	if (!newObject)
	{
		OutputDebugStringA(
			"オブジェクトの生成に失敗しました\n");
		return;
	}
	newObject->Init();

	SceneManager::Instance().AddObject(newObject);

	// 現在選択中のオブジェクト
	SetSelectedObject(newObject);
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
			!objectJson.contains("Scale")||
			!objectJson.contains("Rotation"))
		{
			OutputDebugStringA("必要なデータが不足しているオブジェクトをスキップしました\n");
			continue;
		}

		std::string className = objectJson["Class"].get<std::string>();
		auto obj = KdGameObjectFactory::Instance().CreateGameObject(className);

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

}
