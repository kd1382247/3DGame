#include "EditorManager.h"

#include "../../Framework/GameObject/KdGameObjectFactory.h"

#include"../System/ReferenceManager/ReferenceManager.h"
#include "../Scene/SceneManager.h"
#include"../GameObject/Camera/TPSCamera/TPSCamera.h"



void EditorManager::Draw()
{
	ImGui::Begin("Menu");

	m_stageEditor.Draw();

	DrawModeMenu();

	ImGui::End();

	m_hierarchy.Draw();
	m_inspector.Draw();
}

void EditorManager::DrawModeMenu()
{
	if (IsEditMode())
	{
		ImGui::Text("CurrentMode: Edit");
	}
	else
	{
		ImGui::Text("CurrentMode: Play");
	}

	if (GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		SetEditorMode(EditorMode::Edit);
	}


	if (ImGui::Button("Edit"))
	{
		SetEditorMode(EditorMode::Edit);

		// カメラを削除
		if (m_wpCamera.lock())
		{
			m_wpCamera.lock()->Destroy();
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Play"))
	{
		SetEditorMode(EditorMode::Play);


		if(!m_wpCamera.lock())
		{
			// 一時的にカメラを用意
			std::shared_ptr<TPSCamera>camera = std::make_shared<TPSCamera>();
			camera->Init();
			SceneManager::Instance().AddObject(camera);
			m_wpCamera = camera;
		}


		// 全てのオブジェクトを生成後に関連付け
		ReferenceManager::Instance().AssociateObjects(SceneManager::Instance().GetObjList());
	}
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
