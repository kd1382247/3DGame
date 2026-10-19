#pragma once

#include"../BaseScene/BaseScene.h"

class GameOverUI;
class Setting;

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// ゲームシーン (ステージを読み込んで、ゲームを進める)
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// 状態は Playing → GameOver の一方通行
// ・ゲームオーバー : PlayerDieState が、死亡モーションの終わりに NotifyGameOver() で知らせる
// ・ゲームクリア   : ボス部屋が Cleared になったこと(ボスの撃破演出の終わり)を、毎フレームの確認で見つけて、
//                    演出は挟まずにすぐリザルトへ移動する
// Tabキーで設定画面(Setting)を開く。開いている間は、設定画面以外の更新と時間を止めて、マウスカーソルを出す
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
		GameOver
	};

	void SetupObjectReferences();

	void Event() override;
	void Init()  override;

	// 設定画面を開いている間は、設定画面以外の更新を止める
	// ゲームオーバー中は、敵とボスの更新を止める (倒されている途中のものは、やられモーションを進めるので止めない)
	bool IsUpdateSkipped(const std::shared_ptr<KdGameObject>& obj)const override;

	// ステージを読み込んで、ゲームを始められる状態にする
	void LoadStage();

	// 設定画面(Tabで開く)の毎フレームの処理
	void UpdatePause();

	// 設定画面を開けるか (ゲームオーバーの演出中や、部屋の演出中は開けない)
	bool CanOpenSetting()const;

	// 設定画面の開閉に合わせて、ゲームを止める / 動かす (時間・カーソル・カメラ)
	void SetPaused(bool isPaused);

	// 状態ごとの毎フレームの処理
	void UpdatePlaying();
	void UpdateGameOver();

	// ゲームオーバーの演出を始める
	void StartGameOver();

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

	// 設定画面を開いて、ゲームを止めているか
	bool m_isPaused = false;

	// 前フレームでTabキーが押されていたか (「押した瞬間」を取るため)
	bool m_isPrevTabKey = false;

	// ゲームを始めてからの時間 (秒)。ボスを倒すと止まり、クリアタイムになる
	// (設定画面を開いている間や、ゲームオーバーの演出中は進まない)
	float m_playTime = 0.0f;

	// ゲームオーバーのUI (オブジェクトリストにも入っている。選択結果を聞くために持っておく)
	std::shared_ptr<GameOverUI>  m_spGameOverUI  = nullptr;

	// 設定画面 (オブジェクトリストにも入っている。開閉を見るために持っておく)
	std::shared_ptr<Setting>     m_spSetting     = nullptr;

};
