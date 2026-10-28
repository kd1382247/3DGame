#include "GameScene.h"
#include <chrono>
#include"../SceneManager.h"

#include"../../GameObject/GameResultUI/GameOverUI.h"
#include"../Setting/Setting.h"
#include"../Setting/PauseMenu.h"

#include"../../System/StageDataManager/StageDataManager.h"
#include"../../System/ReferenceManager/ReferenceManager.h"
#include"../../BGMManager/BGMManager.h"
#include"../../GameObject/Character/Enemy/EnemyBase.h"
#include"../../GameObject/Character/Enemy/SpawnableEnemies.h"
#include"../../GameObject/Character/Boss/BossBase.h"
#include"../../GameObject/Character/Player/Player.h"
#include"../../GameObject/EffekseerAssets/EffekseerAssets.h"
#include"../../GameObject/Gimmick/Room/Room.h"
#include"../../GameObject/Gimmick/Room/RoomManager.h"
#include"../../GameObject/Gimmick/Checkpoint/CheckPointManager.h"
#include"../../System/TimeManager/TimeManager.h"
#include"../../System/GameResult/GameResult.h"
#include"../../GameObject/Camera/CameraBase.h"
#include"../../System/Mouse/CursorVisibility.h"

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

	// ステージの読み込みに失敗した時は、タイトルへ戻る
	// (暗転が明けるまでは SetNextScene が受け付けられないので、受け付けられるまで毎フレーム頼む)
	if (m_isLoadFailed)
	{
		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Title);

		return;
	}

	// ポーズメニュー(Tabで開く)
	UpdatePause();

	// ポーズメニューを開いている間は、ゲームの状態(ゲームオーバー・クリアの判定)を進めない
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

	// ---------------------デバッグキー----------------------//

	// タイトル画面
	if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Title);
	}

	if (GetAsyncKeyState('R') & 0x8000)
	{
		// 今までのプレイ時間を、クリアタイムとして渡す
		// (渡さないと、GameResultに前の記録(または初期値の0)が残ったまま、リザルトに表示される)
		GameResult::Instance().SetClearTime(m_playTime);

		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Result);
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
	// ポーズメニューを開いている間は、メニューと設定画面だけ更新する
	if (m_isPaused)
	{
		return obj != m_spSetting && obj != m_spPauseMenu;
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
		// (ここで SetNextScene を呼んでも、暗転の最中なので受け付けられない。Event() が、受け付けられるまで頼み直す)
		KdDebugGUI::Instance().AddErrorLog("Stage load failed : '%s'\n", StageName.c_str());

		m_isLoadFailed = true;

		return;
	}

	// 敵のモデルを先読みする
	// (ゲーム中に初めて敵が出てきた時に、モデルの読み込みで処理が止まらないようにする。
	//  シーンが切り替わった直後は、暗転のまま読み込みが終わるのを待っているので、時間がかかっても見えない)
	PreloadEnemyAssets();

	// エフェクトをロードする
	PreloadEffekseerAssets();

	// 全てのオブジェクトを生成した後に関連付け (エディタのPlayと同じ)
	ReferenceManager::Instance().AssociateObjects(m_objList);

	// チェックポイントを通っていたら、その時点の状態からやり直す
	// (プレイヤーは、最初のPreUpdateより前に移す。PreUpdateが「移動前の位置」として、移した後の位置を覚える)
	RestoreFromCheckPoint();

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

	// ポーズメニュー (Tabで開く。最初は閉じている)
	// (設定画面の後に追加する。同じフレームで、設定画面が閉じたことをメニューが見つけられるようにするため)
	m_spPauseMenu = std::make_shared<PauseMenu>();
	m_spPauseMenu->Init();
	m_spPauseMenu->SetSetting(m_spSetting);

	AddObject(m_spPauseMenu);
}

void GameScene::PreloadEnemyAssets()
{
	using Clock = std::chrono::steady_clock;

	const Clock::time_point totalStart = Clock::now();

	m_preloadedModels.clear();
	m_preloadedModels.reserve(SpawnableEnemies::ModelPaths.size());

	for (const std::string& path : SpawnableEnemies::ModelPaths)
	{
		const Clock::time_point start = Clock::now();

		// KdModelWork::SetModelData(ファイル名) の中で行っているのと同じ読み込み
		// (読み込み済みのモデルなら、すぐに返ってくる)
		const std::shared_ptr<KdModelData> spModelData = KdAssets::Instance().m_modeldatas.GetData(path);

		if (!spModelData)
		{
			KdDebugGUI::Instance().AddErrorLog("Enemy model preload failed : '%s'\n", path.c_str());

			continue;
		}

		// 持っておく (敵を生成する時に、読み込み済みのデータが解放されたままにならないようにする)
		m_preloadedModels.push_back(spModelData);

		// どのモデルが重いかを調べるための、読み込み時間のログ
		const float ms = std::chrono::duration<float, std::milli>(Clock::now() - start).count();

		KdDebugGUI::Instance().AddLog("Preload : %s (%.1f ms)\n", path.c_str(), ms);
	}

	const float totalMs = std::chrono::duration<float, std::milli>(Clock::now() - totalStart).count();

	KdDebugGUI::Instance().AddLog("Preload total : %.1f ms\n", totalMs);
}

void GameScene::PreloadEffekseerAssets()
{
	// ファイルパスからエフェクトをロード
	for (const auto& fileName : EffekseerAssets::EffekseerPaths)
	{
		KdEffekseerManager::GetInstance().LoadEffekseer(fileName);
	}
}

void GameScene::RestoreFromCheckPoint()
{
	const CheckPointManager& checkPointManager = CheckPointManager::Instance();

	// 1つも通っていなければ、ステージに置いた状態から始める
	if (!checkPointManager.HasCheckPoint())
	{
		return;
	}

	// プレイ時間を、チェックポイントに着いた時点に戻す
	m_playTime = checkPointManager.GetPlayTime();

	for (const auto& obj : m_objList)
	{
		// プレイヤーは、チェックポイントの位置へ移す
		if (const auto player = std::dynamic_pointer_cast<Player>(obj))
		{
			// 移動前の位置も同じにする (離れたままだと、最初のフレームで移動したことになる)
			player->SetPos(checkPointManager.GetRespawnPos());
			player->SetPrevPos(checkPointManager.GetRespawnPos());

			continue;
		}

		// クリア条件を満たしていた部屋は、クリア済みの状態にする
		// (倒した敵が元に戻ったり、扉が閉じて、また戦闘が始まったりしないようにする)
		if (const auto room = std::dynamic_pointer_cast<Room>(obj))
		{
			if (checkPointManager.IsRoomCleared(room->GetObjectName()))
			{
				room->RestoreCleared();
			}
		}
	}
}

void GameScene::UpdatePause()
{
	// Tabキー (押した瞬間だけ反応させるため、毎フレーム状態を覚える)
	const bool isTabKey     = (GetAsyncKeyState(VK_TAB) & 0x8000) != 0;
	const bool isTabTrigger = isTabKey && !m_isPrevTabKey;

	m_isPrevTabKey = isTabKey;

	if (!m_spPauseMenu || m_isNextSceneRequested)
	{
		return;
	}

	// Tabで、ポーズメニューを開く / 1つ前の画面へ戻る
	// (操作説明・設定画面 → メニュー → ゲーム)
	if (isTabTrigger)
	{
		if (m_spPauseMenu->IsOpen())
		{
			m_spPauseMenu->Back();
		}
		else if (CanOpenSetting())
		{
			m_spPauseMenu->Open();
		}
	}

	// メニューで TITLE が選ばれたら、タイトルへ移動する
	if (m_spPauseMenu->IsTitleRequested())
	{
		m_isNextSceneRequested = true;

		// 時間が止まったままシーンを移動しないようにする
		// (ゲームの更新は、m_isPaused が true のままなので、動かない)
		TimeManager::Instance().SetPause(false);

		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Title);

		return;
	}

	// ポーズメニューの開閉に合わせて、止める / 動かす
	// (設定画面の×ボタンで閉じた時は、メニューへ戻るだけなので、止めたまま)
	const bool isOpen = m_spPauseMenu->IsOpen();

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

	// ポーズ中はカーソルを表示、ゲームに戻る時は消す
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
	// ・部屋に入った時の演出中(ミッションの文字・ボスの登場演出)は、進めない
	// ・ヒットストップやスローの影響を受けないように、実時間の経過を使う
	if (!RoomManager::Instance().IsIntro() && !RoomManager::Instance().IsBossDefeat())
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

	// 次のシーンへ進むことを頼んだ後は、何もしない
	// (シーンが切り替わるまでの間も、この関数は呼ばれる。
	//  ここでカーソルを出し直すと、リトライで消したカーソルが残ってしまう)
	if (m_isNextSceneRequested)
	{
		return;
	}

	// ボタンを選べるようになったら、カーソルを出してカメラの回転を止める
	// (カメラは毎フレームカーソルを画面中央へ戻すので、止めないとボタンを押せない)
	// (SetCursorVisible は、毎フレーム呼んでも状態が変わらない)
	if (m_spGameOverUI->IsMenuOpen())
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
		// リトライでゲームに戻るので、カーソルを消す
		SetCursorVisible(false);

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
