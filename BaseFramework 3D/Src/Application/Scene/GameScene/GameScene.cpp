#include "GameScene.h"
#include"../SceneManager.h"

#include"../../GameObject/GameResultUI/GameOverUI.h"
#include"../Setting/Setting.h"

#include"../../System/StageDataManager/StageDataManager.h"
#include"../../System/ReferenceManager/ReferenceManager.h"
#include"../../BGMManager/BGMManager.h"
#include"../../GameObject/Character/Enemy/EnemyBase.h"
#include"../../GameObject/Character/Boss/BossBase.h"
#include"../../GameObject/Character/Player/Player.h"
#include"../../GameObject/Gimmick/Room/Room.h"
#include"../../GameObject/Gimmick/Room/RoomManager.h"
#include"../../System/TimeManager/TimeManager.h"
#include"../../System/GameResult/GameResult.h"
#include"../../GameObject/Camera/CameraBase.h"

namespace
{
	// ゲームで読み込むステージ名 (Asset/Data/Stage/ の中のフォルダー名)
	// ステージ選択を作るまでは、ここで固定する
	const std::string StageName = "Stage02";

	// 演出が始まる時の効果音のファイル名 (素材ができたら入れる。空のままなら鳴らさない)
	const std::string GameOverSE = "";

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

	// 設定画面を開いたままシーンを移動しても、時間が止まったままにならないようにする
	TimeManager::Instance().SetPause(false);
}

void GameScene::NotifyGameOver()
{
	// ゲームオーバーの演出中や、クリアでシーンを移動する時は受け付けない
	if (m_gameState != GameState::Playing || m_isNextSceneRequested)
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

	// 設定画面(Tabで開く)
	UpdatePause();

	// 設定画面を開いている間は、ゲームの状態(ゲームオーバー・クリアの判定)を進めない
	if (!m_isPaused)
	{
		switch (m_gameState)
		{
		case GameState::Playing:
			UpdatePlaying();
			break;

		case GameState::GameOver:
			UpdateGameOver();
			break;
		}
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
	// 設定画面を開いている間は、設定画面だけ更新する
	if (m_isPaused)
	{
		return obj != m_spSetting;
	}

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

	// 設定画面 (Tabで開く。最初は閉じている)
	m_spSetting = std::make_shared<Setting>();
	m_spSetting->Init();

	AddObject(m_spSetting);

	// ゲーム中はマウスカーソルを隠す
	SetCursorVisible(false);
}

void GameScene::UpdatePause()
{
	// Tabキー (押した瞬間だけ反応させるため、毎フレーム状態を覚える)
	const bool isTabKey     = (GetAsyncKeyState(VK_TAB) & 0x8000) != 0;
	const bool isTabTrigger = isTabKey && !m_isPrevTabKey;

	m_isPrevTabKey = isTabKey;

	if (!m_spSetting)
	{
		return;
	}

	// Tabで、設定画面を開く / 閉じる
	if (isTabTrigger)
	{
		if (m_spSetting->IsOpen())
		{
			m_spSetting->Close();
		}
		else if (CanOpenSetting())
		{
			m_spSetting->Open();
		}
	}

	// 設定画面の開閉に合わせて、止める / 動かす
	// (設定画面の×ボタンで閉じた時も、ここで気づく)
	const bool isOpen = m_spSetting->IsOpen();

	if (isOpen != m_isPaused)
	{
		SetPaused(isOpen);
	}
}

bool GameScene::CanOpenSetting() const
{
	// ゲームオーバーの演出中や、クリアでシーンを移動する時は開けない
	if (m_gameState != GameState::Playing || m_isGameOverRequested || m_isNextSceneRequested)
	{
		return false;
	}

	// 部屋に入った時の演出と、ボスを倒した演出の間は開けない
	// (カメラの演出などは止められないので、終わるまで待つ)
	if (RoomManager::Instance().IsIntro() || RoomManager::Instance().IsBossDefeat())
	{
		return false;
	}

	return true;
}

void GameScene::SetPaused(bool isPaused)
{
	m_isPaused = isPaused;

	// 時間を止める / 動かす
	TimeManager::Instance().SetPause(isPaused);

	// 開いている間はカーソルを出す (カメラは毎フレームカーソルを画面中央へ戻すので、回転も止める)
	SetCursorVisible(isPaused);

	SetCameraRotateLock(isPaused);

	// 閉じる時、閉じるために押したマウスのボタンが、そのままプレイヤーの攻撃にならないようにする
	// (押している間は受け付けず、離したら受け付けるようにする)
	if (!isPaused)
	{
		for (const auto& obj : m_objList)
		{
			const auto player = std::dynamic_pointer_cast<Player>(obj);

			if (player)
			{
				player->BlockMouseInput();
			}
		}
	}
}

void GameScene::UpdatePlaying()
{
	// クリアタイムを計る (ボスを倒した瞬間に止める)
	// ・設定画面を開いている間は、この関数が呼ばれないので、進まない
	// ・ヒットストップやスローの影響を受けないように、実時間の経過を使う
	if (!RoomManager::Instance().IsBossDefeat())
	{
		m_playTime += TimeManager::Instance().GetUnscaleeDeltaTime();
	}

	// プレイヤーが死んだ (クリアと同時なら、ゲームオーバーを優先する)
	if (m_isGameOverRequested)
	{
		StartGameOver();

		return;
	}

	// ボス部屋がクリアされた (ボスの撃破演出が終わった)
	// クリアの演出は無しで、すぐにリザルトへ進む
	if (!m_isNextSceneRequested && IsBossRoomCleared())
	{
		m_isNextSceneRequested = true;

		// リザルトで表示するために、クリアタイムを記録しておく
		GameResult::Instance().SetClearTime(m_playTime);

		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Result);
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
