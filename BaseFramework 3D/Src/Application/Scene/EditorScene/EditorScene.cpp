#include "EditorScene.h"

#include"../../Editor/EditorManager.h"
#include"../../GameObject/Camera/EditorCamera/EditorCamera.h"

#include"../../System/WayPointManager/WayPointManager.h"
#include"../../GameObject/WayPoint/WayPoint.h"
#include"../../GameObject/Stage/Stage01/Collision/WallCollision/WallCollisionManager.h"


void EditorScene::EditorUpdate()
{
	if (m_spEditorCamera)
	{
		m_spEditorCamera->Update();
	}
}

void EditorScene::PreDraw()
{
	// EditModeならSceneViewへ描画
	if (EditorManager::Instance().IsEditMode())
	{
		EditorManager::Instance().BeginSceneViewRender();
	}

	BaseScene::PreDraw();

	if (EditorManager::Instance().IsEditMode() && m_spEditorCamera)
	{
		m_spEditorCamera->PreDraw();
	}
}

void EditorScene::DrawDebug()
{

	BaseScene::DrawDebug();

	WallCollisionManager::Instance().DrawDebug();

	WayPointManager::Instance().DrawDebug();
	
	// SceneViewへの描画終了
	if (EditorManager::Instance().IsEditMode())
	{
		EditorManager::Instance().EndSceneViewRender();
	}
}

void EditorScene::BackupObjectList()
{
	m_spBackupList = std::move(m_objList);
	m_objList.clear();
}

void EditorScene::RestoreObjectList()
{
	m_objList = std::move(m_spBackupList);
	m_spBackupList.clear();
}

void EditorScene::ClearBackupList()
{
	m_spBackupList.clear();
}

bool EditorScene::UsePostProcess() const
{

	return !EditorManager::Instance().IsEditMode();

}

void EditorScene::SetupObjectReferences()
{

}

void EditorScene::Event()
{

	////// 現在のオブジェクト数をデバッグ
	KdDebugGUI::Instance().ClearLog();
	KdDebugGUI::Instance().AddLog("object%d", GetObjList().size());

	m_spEditorCamera->Update();
}

void EditorScene::Init()
{

	m_spEditorCamera = std::make_shared<EditorCamera>();
	m_spEditorCamera->Init();

	EditorManager::Instance().SetEditorCamera(m_spEditorCamera);
}
