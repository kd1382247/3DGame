#include "EditorScene.h"

#include"../SceneManager.h"

#include"../../Editor/EditorManager.h"
#include"../../GameObject/Camera/EditorCamera/EditorCamera.h"

#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"

#include"../../GameObject/Character/Player/Player.h"

#include"../../GameObject/Terrains/Ground/Ground.h"

#include"../../GameObject/Terrains/Box.h"

#include"../../System/PathFinding/WayPointManager.h"
#include"../../System/PathFinding/WayPoint/WayPoint.h"

// 敵
#include"../../GameObject/Character/Enemy/Beholder/Beholder.h"
#include"../../GameObject//Character/Enemy/Bomb/Bomb.h"
#include"../../GameObject//Character/Enemy/Cactas/Cactas.h"
#include"../../GameObject//Character/Enemy/ChestMonster/ChestMonster.h"
#include"../../GameObject//Character/Enemy/Golem/Golem.h"
#include"../../GameObject//Character/Enemy/Mage/Mage.h"
#include"../../GameObject//Character/Enemy/Mushroom/Mushroom.h"
#include"../../GameObject//Character/Enemy/Slime/Slime.h"
#include"../../GameObject//Character/Enemy/StarFish/StarFish.h"
#include"../../GameObject//Character/Enemy/Swarm/Swarm.h"
#include"../../GameObject//Character/Enemy/TurtleShell/TurtleShell.h"

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
	KdDebugGUI::Instance().AddLog("object%d", WayPointManager::Instance().GetWayPoints().size());

	m_spEditorCamera->Update();
}

void EditorScene::Init()
{
	//===================================================================
	// ステージ初期化
	//===================================================================
	std::shared_ptr<Ground>_ground = std::make_shared<Ground>();
	_ground->Init();
	AddObject(_ground);

	std::shared_ptr<Box>_box = std::make_shared<Box>();
	_box->Init();
	AddObject(_box);

	//===================================================================
	// キャラクター初期化
	//===================================================================
	std::shared_ptr<Player> _player = std::make_shared<Player>();
	_player->Init();
	AddObject(_player);

	//===================================================================
	// カメラ初期化
	//===================================================================
	m_camera = std::make_shared<TPSCamera>();
	m_camera->Init();
	m_camera->SetTarget(_player);
	AddObject(m_camera);

	m_spEditorCamera = std::make_shared<EditorCamera>();
	m_spEditorCamera->Init();

	_player->SetCamera(m_camera);
}
