#include "GameScene.h"
#include"../SceneManager.h"

#include"../../Editor/EditorManager.h"
#include"../../GameObject/Camera/EditorCamera/EditorCamera.h"

#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"

#include"../../GameObject/Character/Player/Player.h"

#include"../../GameObject/Terrains/Ground/Ground.h"

#include"../../GameObject/Terrains/Box.h"

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



void GameScene::EditorUpdate()
{
	OutputDebugStringA("GameScene EditorUpdate\n");

	if (m_spEditorCamera)
	{
		m_spEditorCamera->Update();
	}
	
}

void GameScene::PreDraw()
{
	BaseScene::PreDraw();

	if (EditorManager::Instance().IsEditMode() && m_spEditorCamera)
	{
		m_spEditorCamera->PreDraw();
	}
}

void GameScene::SetupObjectReferences()
{/*
	std::shared_ptr<Player> player=FindObject<Player>("Player");
	std::shared_ptr<TPSCamera> camera=FindObject<TPSCamera>("TPSCamera");
	std::shared_ptr<Ground> ground=FindObject<Ground>("Ground");


	player->RegistHitObject(ground);
	camera->SetTarget(player);*/
}
void GameScene::Event()
{

	////// 現在のオブジェクト数をデバッグ
	KdDebugGUI::Instance().ClearLog();
	KdDebugGUI::Instance().AddLog("object%d", m_objList.size());

	m_spEditorCamera->Update();
}

void GameScene::Init()
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
	// エネミー初期化
	//===================================================================
	// Beholder
	std::shared_ptr<Beholder>_beholder = std::make_shared<Beholder>();
	_beholder->Init();
	AddObject(_beholder);

	// Bomb
	std::shared_ptr<Bomb>_bomb = std::make_shared<Bomb>();
	_bomb->Init();
	AddObject(_bomb);

	// Cactas
	std::shared_ptr<Cactas>_cactas = std::make_shared<Cactas>();
	_cactas->Init();
	AddObject(_cactas);

	// ChestMonster
	std::shared_ptr<ChestMonster>_chestMonster = std::make_shared<ChestMonster>();
	_chestMonster->Init();
	AddObject(_chestMonster);

	// Golem
	std::shared_ptr<Golem>_golem = std::make_shared<Golem>();
	_golem->Init();
	AddObject(_golem);

	// Mage
	std::shared_ptr<Mage>_mage = std::make_shared<Mage>();
	_mage->Init();
	AddObject(_mage);

	// Mushroom
	std::shared_ptr<Mushroom>_mushroom = std::make_shared<Mushroom>();
	_mushroom->Init();
	AddObject(_mushroom);

	// Slime
	std::shared_ptr<Slime>_slime = std::make_shared<Slime>();
	_slime->Init();
	AddObject(_slime);

	// StarFish
	std::shared_ptr<StarFish>_starFish = std::make_shared<StarFish>();
	_starFish->Init();
	AddObject(_starFish);

	// Swarm
	std::shared_ptr<Swarm>_swarm = std::make_shared<Swarm>();
	_swarm->Init();
	AddObject(_swarm);

	// TurtleShell
	std::shared_ptr<TurtleShell>_turtleShell = std::make_shared<TurtleShell>();
	_turtleShell->Init();
	AddObject(_turtleShell);


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
