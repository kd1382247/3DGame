#include "EditorScene.h"

#include"../SceneManager.h"

#include"../../Editor/EditorManager.h"
#include"../../GameObject/Camera/EditorCamera/EditorCamera.h"


#include"../../System/PathFinding/WayPointManager.h"
#include"../../System/PathFinding/WayPoint/WayPoint.h"


void EditorScene::EditorUpdate()
{
	OutputDebugStringA("GameScene EditorUpdate\n");

	if (m_spEditorCamera)
	{
		m_spEditorCamera->Update();
	}
}

void EditorScene::PreDraw()
{
	BaseScene::PreDraw();

	if (EditorManager::Instance().IsEditMode() && m_spEditorCamera)
	{
		m_spEditorCamera->PreDraw();
	}
}

void EditorScene::DrawDebug()
{
	BaseScene::DrawDebug();

	WayPointManager::Instance().DrawDebug();
}

void EditorScene::ClearObjectList()
{
	m_spBackupList = std::move(m_objList);
	m_objList.clear();
}

void EditorScene::RestoreObjList()
{
	m_objList = std::move(m_spBackupList);
	m_spBackupList.clear();
}

void EditorScene::ClearBackupList()
{
	m_spBackupList.clear();
}

void EditorScene::SetupObjectReferences()
{}

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
}
