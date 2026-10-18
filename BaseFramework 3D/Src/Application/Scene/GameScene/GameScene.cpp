#include "GameScene.h"
#include"../SceneManager.h"

#include"../../GameObject/GameResultUI/GameOverUI.h"
#include"../../GameObject/GameResultUI/GameClearUI.h"

#include"../../System/StageDataManager/StageDataManager.h"
#include"../../System/ReferenceManager/ReferenceManager.h"
#include"../../BGMManager/BGMManager.h"
#include"../../GameObject/Character/Enemy/EnemyBase.h"
#include"../../GameObject/Character/Boss/BossBase.h"
#include"../../GameObject/Gimmick/Room/Room.h"
#include"../../GameObject/Camera/CameraBase.h"

namespace
{
	// ゲームで読み込むステージ名 (Asset/Data/Stage/ の中のフォルダー名)
	// ステージ選択を作るまでは、ここで固定する
	const std::string StageName = "Stage02";

	// 演出が始まる時の効果音のファイル名 (素材ができたら入れる。空のままなら鳴らさない)
	const std::string GameOverSE  = "";
	const std::string GameClearSE = "";

	// ゲーム中にデバッグ線(当たり判定の球など)を描くか
	// (開発中に確認したい時だけ true にする)
	constexpr bool ShowDebugWire = false;

	// 効果音を鳴らす (ファイル名が空なら何もしない)
	void PlaySE(const std::string& fileName)
	{
		if (fileName.empty())
		{
			return;
		}

		KdAudioManager::Instance().PlaySE(fileName);
	}
}

GameScene::~GameScene()
{
	// 隠したままシーンを移動すると、タイトルなどでカーソルが見えなくなるので戻す
	SetCursorVisible(true);
}

void GameScene::NotifyGameOver()
{
	// ゲームオーバーやクリアの演出中は受け付けない
	if (m_gameState != GameState::Playing)
	{
		return;
	}

	m_isGameOverRequested = true;
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
	// 最初の1フレームだけステージを読み込む
	// (Init()の中で読み込むと、シーンが切り替わる前に呼ばれてしまい、
	//  StageDataManagerが追加するオブジェクトが前のシーンに入ってしまう)
	if (!m_isStageLoaded)
	{
		m_isStageLoaded = true;

		LoadStage();
	}

	switch (m_gameState)
	{
	case GameState::Playing:
		UpdatePlaying();
		break;

	case GameState::GameOver:
		UpdateGameOver();
		break;

	case GameState::GameClear:
		UpdateGameClear();
		break;
	}

	if (GetAsyncKeyState('G') & 0x8000)
	{
		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Title);
	}


}

void GameScene::Init()
{
	// ゲーム中は、デバッグ線(当たり判定の球など)を描かない
	// (シーンが切り替わる時に、SceneManager::ResetSystems() で元に戻る)
	KdDebugWireFrame::SetEnable(ShowDebugWire);
}

bool GameScene::IsUpdateSkipped(const std::shared_ptr<KdGameObject>& obj) const
{
	if (m_gameState != GameState::GameOver)
	{
		return false;
	}

	// 倒されている途中の敵は、やられモーションを最後まで進める (止めると、倒れたまま残ってしまう)
	if (const auto enemy = std::dynamic_pointer_cast<EnemyBase>(obj))
	{
		return !enemy->GetHealth().IsDead();
	}

	if (const auto boss = std::dynamic_pointer_cast<BossBase>(obj))
	{
		return !boss->GetHealth().IsDead();
	}

	return false;
}

void GameScene::LoadStage()
{
	// ステージのJsonを読み込んで、オブジェクトを生成する
	if (!StageDataManager::Instance().Load(StageName))
	{
		// 読み込みに失敗したらタイトルへ戻る
		KdDebugGUI::Instance().AddErrorLog("Stage load failed : '%s'\n", StageName.c_str());

		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Title);

		return;
	}

	// 全てのオブジェクトを生成した後に関連付け (エディタのPlayと同じ)
	ReferenceManager::Instance().AssociateObjects(m_objList);

	// 読み込んだオブジェクトに、最初のPreUpdateを実行する
	// (読み込みはUpdateの途中なので、このフレームのPreUpdateはもう終わっていて、そのままだと飛ばされる。
	//  キャラクターはPreUpdateで「移動前の位置」を覚えていて、当たり判定はそこからの移動で位置を決め直す。
	//  飛ばされると移動前の位置が原点のままなので、プレイヤーやボスが原点に戻されてしまう)
	const std::list<std::shared_ptr<KdGameObject>> loadedObjList = m_objList;

	for (const auto& obj : loadedObjList)
	{
		obj->PreUpdate();
	}

	// ゲーム中はマウスカーソルを隠す
	SetCursorVisible(false);
}

void GameScene::UpdatePlaying()
{
	// プレイヤーが死んだ (クリアと同時なら、ゲームオーバーを優先する)
	if (m_isGameOverRequested)
	{
		StartGameOver();

		return;
	}

	// ボス部屋がクリアされた
	if (IsBossRoomCleared())
	{
		StartGameClear();
	}
}

void GameScene::UpdateGameOver()
{
	// 敵とボスは行動を止めて、待機モーションにする
	// (ゲームオーバー中に新しく出てきた敵にも効くように、毎フレーム行う)
	HoldEnemiesIdle();

	if (!m_spGameOverUI)
	{
		return;
	}

	// ボタンを選べるようになったら、カーソルを出してカメラの回転を止める (1回だけ)
	// (カメラは毎フレームカーソルを画面中央へ戻すので、止めないとボタンを押せない)
	if (m_spGameOverUI->IsMenuOpen() && m_isCursorHidden)
	{
		SetCursorVisible(true);

		SetCameraRotateLock(true);
	}

	// 選ばれるまで待つ
	const GameOverUI::Choice choice = m_spGameOverUI->GetChoice();

	if (choice == GameOverUI::Choice::None || m_isNextSceneRequested)
	{
		return;
	}

	m_isNextSceneRequested = true;

	if (choice == GameOverUI::Choice::Retry)
	{
		// 同じシーンでも、新しく作り直される (前の状態は ResetSystems() で消える)
		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Game);
	}
	else
	{
		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Title);
	}
}

void GameScene::UpdateGameClear()
{
	if (!m_spGameClearUI || m_isNextSceneRequested)
	{
		return;
	}

	// 演出が終わったらリザルトへ
	if (m_spGameClearUI->IsFinished())
	{
		m_isNextSceneRequested = true;

		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Result);
	}
}

void GameScene::StartGameOver()
{
	m_gameState = GameState::GameOver;

	// ゲームオーバーのBGMに切り替える (シーンが切り替わる時に BGMManager::Reset() で解除される)
	BGMManager::Instance().SetOverride(BGMManager::BGMType::GameOver);

	PlaySE(GameOverSE);

	m_spGameOverUI = std::make_shared<GameOverUI>();
	m_spGameOverUI->Init();

	// 最後に追加する (オブジェクトリストの順に描画されるので、他のUIの上に重なる)
	AddObject(m_spGameOverUI);
}

void GameScene::StartGameClear()
{
	m_gameState = GameState::GameClear;

	// ゲームクリアのBGMに切り替える (シーンが切り替わる時に BGMManager::Reset() で解除される)
	BGMManager::Instance().SetOverride(BGMManager::BGMType::GameClear);

	PlaySE(GameClearSE);

	m_spGameClearUI = std::make_shared<GameClearUI>();
	m_spGameClearUI->Init();

	// 最後に追加する (オブジェクトリストの順に描画されるので、他のUIの上に重なる)
	AddObject(m_spGameClearUI);
}

void GameScene::HoldEnemiesIdle()
{
	for (const auto& obj : m_objList)
	{
		// 倒されている途中のものは、IsUpdateSkipped() と同じように対象外にする
		if (const auto enemy = std::dynamic_pointer_cast<EnemyBase>(obj))
		{
			if (!enemy->GetHealth().IsDead())
			{
				enemy->HoldIdle();
			}
		}
		else if (const auto boss = std::dynamic_pointer_cast<BossBase>(obj))
		{
			if (!boss->GetHealth().IsDead())
			{
				boss->HoldIdle();
			}
		}
	}
}

bool GameScene::IsBossRoomCleared() const
{
	for (const auto& obj : m_objList)
	{
		const auto room = std::dynamic_pointer_cast<Room>(obj);

		if (room && room->GetRoomType() == Room::RoomType::Boss && room->IsCleared())
		{
			return true;
		}
	}

	return false;
}

void GameScene::SetCameraRotateLock(bool isLock)
{
	for (const auto& obj : m_objList)
	{
		const auto camera = std::dynamic_pointer_cast<CameraBase>(obj);

		if (camera)
		{
			camera->SetMouseRotateLock(isLock);
		}
	}
}

void GameScene::SetCursorVisible(bool isVisible)
{
	// 今の状態と同じなら何もしない (ShowCursorの回数がずれないようにする)
	if (m_isCursorHidden == !isVisible)
	{
		return;
	}

	ShowCursor(isVisible);

	m_isCursorHidden = !isVisible;
}
