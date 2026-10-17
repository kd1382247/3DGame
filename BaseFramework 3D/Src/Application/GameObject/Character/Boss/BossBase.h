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

private:

	


protected:

	// パラメータクラスのインスペクター描画。各Enemyが実装する
	virtual void DrawParameterInspector() = 0;

	// HPが最大値の半分以下かどうか(全ボス共通の第2フェーズ判定)
	bool IsSecondPhase()const;

	// 重み付き抽選。weights[i]が0以下の要素は選ばれない。戻り値は選ばれたweightsのindex
	int LotteryPattern(const std::vector<float>& weights)const;


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
};