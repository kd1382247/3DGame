#pragma once

#include"../../Character/CharacterBase.h"

class Player;

class TPSCamera;

class BossBase :public CharacterBase
{

public:

	void Init()override;

	void PostUpdate()override;
	void SetUpReference()override;

	void DrawInspector()override;

	void OnHit(const AttackInfo attackInfo)override;
	
	void UpdateMove();
	
	void SetTargetDir();

	// 行動を止めて、待機モーションにする
	// (ゲームオーバー中など、Updateを止めている間、毎フレーム呼ぶ)
	void HoldIdle();

	bool HasReachedTarget()const { return m_hasReachedTarget; }


	// =====================ボスの登場演出=====================//
	virtual void StartIntro() {}

	// イントロ開始
	bool IsIntroRequested()const { return m_introRequested; }

	// カメラをボスに寄らせる
	void StartCameraIntro();

	// ボスの咆哮開始
	void StartCameraRoar();

	// カメラがボスに寄って止まっているか
	bool IsCameraIntroHold();

	// 演出中ならtrueを返す
	bool IsBossIntroPlaying();


	// =====================ボスの撃破演出(枠組み)=====================//
	// 死亡アニメーションと同時に始まり、死亡アニメーションが終わると終わる。
	// カメラは、ボスの正面へ回り込んで、よろけるボスを映す(TPSCamera::StartBossDefeat)。
	// カメラ以外の演出を足したいボスは、overrideして、BossBaseの関数も呼ぶ。
	// 呼ばれる流れ: Start(死亡開始) → Update(毎フレーム) → End(死亡アニメ終了)
	virtual void StartDefeatDirection();
	virtual void UpdateDefeatDirection() {}
	virtual void EndDefeatDirection();


	void PlayDeathSmoke();

private:

	


protected:

	// パラメータクラスのインスペクター描画。各Enemyが実装する
	virtual void DrawParameterInspector() = 0;

	// HPが最大値の半分以下かどうか(全ボス共通の第2フェーズ判定)
	bool IsSecondPhase()const;

	// 重み付き抽選。weights[i]が0以下の要素は選ばれない。戻り値は選ばれたweightsのindex
	int LotteryPattern(const std::vector<float>& weights)const;

	void PlayHitEffect();

	// 被弾時の共通処理(HPを減らす・ダメージ表示・ヒットエフェクト・撃破時のヒットストップとスロー)
	// 戻り値: この攻撃で倒されたかどうか
	bool ApplyDamage(const AttackInfo& attackInfo);


	// UpdateMoveから呼ばれる。歩き/待機モーションの再生は各Bossが実装する
	virtual void PlayWalkAnimation() {}
	virtual void PlayIdleAnimation() {}


	std::weak_ptr<Player>m_wpPlayer;

	std::weak_ptr<TPSCamera>m_wpCamera;

	const std::string m_flyTextPath = "DamageNumber_Orange.png";

	// プレイヤーに到達したかどうか
	bool m_hasReachedTarget = false;
	// 到達判定の距離(この距離まで近づいたら「到達」とみなし、攻撃を開始する)
	float m_reachDistance = 5.0f;

	float m_reachDistanceMargin = 6.0f;

	bool  m_introRequested = false;


	bool          m_initPos = false;
	Math::Vector3 m_roomCenter = {};

};