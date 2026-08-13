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

	//m_camera = std::make_shared<TPSCamera>();
	//m_camera->Init();
	//AddObject(m_camera);

	m_spEditorCamera = std::make_shared<EditorCamera>();
	m_spEditorCamera->Init();
}
