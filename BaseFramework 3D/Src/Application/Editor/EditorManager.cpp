#include "EditorManager.h"

#include "../../Framework/GameObject/KdGameObjectFactory.h"

#include"../System/WayPointManager/WayPointManager.h"
#include"../System/ReferenceManager/ReferenceManager.h"
#include "../Scene/SceneManager.h"
#include"../GameObject/Camera/TPSCamera/TPSCamera.h"

#include"../System/StageDataManager/StageDataManager.h"


void EditorManager::Draw()
{


	if(m_editorMode==EditorMode::Edit)
	{
		ImGui::Begin("Menu");

		m_stageEditor.Draw();

		DrawModeMenu();

		ImGui::End();

		m_hierarchy.Draw();
		m_inspector.Draw();
	}

	if (m_editorMode == EditorMode::Play)
	{
		if (GetAsyncKeyState(VK_TAB) & 0x8000)
		{
			StopPlayMode();
		}
	}
}

void EditorManager::StartPlayMode()
{

	if (!StageDataManager::Instance().SaveTemporary())
	{
		return;
	}

	// モードを切り替える
	SetEditorMode(EditorMode::Play);

	if (!m_wpCamera.lock())
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

void EditorManager::StopPlayMode()
{

	// Play中の状態を削除
	SceneManager::Instance().ClearObjectList();
	WayPointManager::Instance().ClearWayPoints();

	// Edit開始前の状態を復元
	if (!StageDataManager::Instance().LoadTemporary())
	{
		// ロードに失敗したため復元する
		SceneManager::Instance().RestoreObjList();
		WayPointManager::Instance().RestoreWayPoints();

		return;
	}

	// バックアップリストをクリア
	SceneManager::Instance().ClearBackupList();
	WayPointManager::Instance().ClearBackup();

	// モードを切り替える
	SetEditorMode(EditorMode::Edit);

	// カメラを削除
	if (m_wpCamera.lock())
	{
		m_wpCamera.lock()->Destroy();
	}
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

	if (ImGui::Button("Play"))
	{
		StartPlayMode();
	}
}

void EditorManager::CreateGameObject(const std::string& className)
{
	auto newObject =
		KdGameObjectFactory::Instance().CreateGameObject(className);

	if (!newObject)
	{
		OutputDebugStringA("オブジェクトの生成に失敗しました\n");
		return;
	}
	newObject->Init();

	SceneManager::Instance().AddObject(newObject);

	// 現在選択中のオブジェクト
	SetSelectedObject(newObject);
}
