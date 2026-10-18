#pragma once

#include"../BaseScene/BaseScene.h"

class GameOverUI;
class GameClearUI;

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// ゲームシーン (ステージを読み込んで、ゲームを進める)
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// 状態は Playing → GameOver か GameClear の一方通行
// ・ゲームオーバー : PlayerDieState が、死亡モーションの終わりに NotifyGameOver() で知らせる
// ・ゲームクリア   : ボス部屋が Cleared になったことを、毎フレームの確認で見つける
// エディタのPlayでは GameScene が使われない (GetCurrentScene<GameScene>() が nullptr になる)ので、
// NotifyGameOver() を呼んでも何も起きない
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class GameScene : public BaseScene
{
public :

	GameScene()  { Init(); }
	~GameScene();

	// プレイヤーが死んだことを知らせる (ゲームオーバーの演出が始まる)
	// プレイヤーの更新の途中で呼ばれるので、実際の処理は次のフレームで行う
	void NotifyGameOver();

private:

	enum class GameState
	{
		Playing,
		GameOver,
		GameClear
	};

	void SetupObjectReferences();

	void Event() override;
	void Init()  override;

	// ゲームオーバー中は、敵とボスの更新を止める (倒されている途中のものは、やられモーションを進めるので止めない)
	bool IsUpdateSkipped(const std::shared_ptr<KdGameObject>& obj)const override;

	// ステージを読み込んで、ゲームを始められる状態にする
	void LoadStage();

	// 状態ごとの毎フレームの処理
	void UpdatePlaying();
	void UpdateGameOver();
	void UpdateGameClear();

	// ゲームオーバー / ゲームクリアの演出を始める
	void StartGameOver();
	void StartGameClear();

	// 敵とボスを待機モーションにする (ゲームオーバー中、毎フレーム呼ぶ)
	void HoldEnemiesIdle();

	// ボス部屋がクリアされたか
	bool IsBossRoomCleared()const;

	// カメラのマウス回転を止める / 再開する
	void SetCameraRotateLock(bool isLock);

	// マウスカーソルの表示 / 非表示
	// (ShowCursorは呼んだ回数だけ増減するので、今の状態を覚えておいて回数を合わせる)
	void SetCursorVisible(bool isVisible);

	// 今のゲームの状態
	GameState m_gameState = GameState::Playing;

	// プレイヤーが死んだと知らされたか (次のフレームで、ゲームオーバーを始める)
	bool m_isGameOverRequested = false;

	// 次のシーンへ進むことを、もう頼んだか
	bool m_isNextSceneRequested = false;

	// ステージを読み込み済みか
	// (読み込みは、シーンが切り替わった後の最初のフレームで行う)
	bool m_isStageLoaded = false;

	// マウスカーソルを隠しているか
	bool m_isCursorHidden = false;

	// 演出のUI (オブジェクトリストにも入っている。選択結果や終了を聞くために持っておく)
	std::shared_ptr<GameOverUI>  m_spGameOverUI  = nullptr;
	std::shared_ptr<GameClearUI> m_spGameClearUI = nullptr;

};
