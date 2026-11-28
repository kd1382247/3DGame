#pragma once

#include"../BossBase.h"

#include"Animation/BeholderAnimation.h"
#include"Parameter/BeholderParameter.h"
#include"../../StateMachine/StateMachine.h"


#include"SparkAttack/BeholderSparkAttack.h"
#include"ChargeShot/BeholderChargeShot.h"


class Beholder :public BossBase
{
public:

	Beholder(){}
	~Beholder()override{}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void OnHit(const AttackInfo attackInfo)override;

	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }
	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

	void PlayAnimation(BeholderAnimationType type);

	//==========攻撃==========//

	// SparkBullet関連
	void CreateSparkBullet();
	void FireSparkBullet();
	bool IsStandbySparkBullet();
	void CancelSparkBullet();

	// ChargeSparkBullet関連
	void CreateChargeSparkBullet();
	void FireChargeSparkBullet();
	bool IsStandbyChargeSparkBullet();
	void CancelChargeSparkBullet();

	bool IsAttack()const { return m_attackFlg; }
	void UpdateAttack();
	void EndAttack();


	// Beholderの攻撃パターン
	enum class BeholderAttackPattern
	{
		SparkAttack,
		ChargeShot
	};

	// 攻撃パターンを返す
	BeholderAttackPattern SelectAttackPattern();


	//==========混乱==========//

	void StartDizzy();
	void UpdateDizzy();

	// 浮遊状態に復帰したのか
	bool IsFloatRecovered()const { return m_dizzyState == DizzyState::FloatRecovered; }

	// 目をプレイヤーに追従させるかをセット
	void SetEyeTracking(bool flg) { m_isEyeTracking = flg; }

	// 混乱するカウントを上げる
	void DizzyCountUp();

	//==========アニメーション==========//

	bool IsAnimationFinished()const { return m_animation.IsFinished(); }
	float AnimProgress()      const { return m_animation.AnimProgress(); }

	BeholderAnimationType GetPlayAnimType()const { return m_animation.GetPlayAnimType(); }

	void CreateHPBar();

private:


	// パラメータの適用
	void ApplyParameter();

	//==========混乱==========//

	// 落下中
	void UpdateFalling();

	// 混乱中のタイマーを減らす
	void UpdateDizzyTimeCountDown();

	// 元の高さに戻る
	void UpdateReturnIngHeight();


	void PlayWalkAnimation()override;
	void PlayIdleAnimation()override;

	void DrawParameterInspector()override;

	void UpdateAnimation();

	Math::Quaternion CalcEyeTargetRotation(const Math::Matrix& eyeWorldMat);

	// モデルの目の回転(X軸)
	void UpdateEyeRotationX();

private:

	enum class DizzyState
	{
		Falling,  // 落下中
		Dizzy,    // 混乱
		Returning,// 元の高さに戻る
		FloatRecovered
	};

	// 攻撃クラス(SparkBullet)
	BeholderSparkAttack m_sparkAttack;

	// 攻撃クラス(ChargeSparkBullet)
	BeholderChargeShot m_chargeShot;

	// アニメーションクラス
	BeholderAnimation   m_animation;
	// パラメータークラス
	BeholderParameter   m_parameter;

	// ステートマシン
	StateMachine<Beholder> m_stateMachine;

	//==========混乱==========//

	DizzyState m_dizzyState = DizzyState::Falling;

	// 混乱状態の時間
	float m_dizzyDuration = 5.0f;
	float m_dizzyTime = 0.0f;

	// 元の位置に復帰するまでの時間
	float m_floatRecoveryTime = 0.0f;

	// 浮遊状態に復帰したのか
	bool m_isFloatRecovered = false;

	// 元々いた高さの座標
	float m_originalPosY=0.0f;

	// 復帰開始時の高さ
	float m_fallStartY = 0.0f;

	float m_t = 0.0f;

	// 混乱するまでのカウンター
	int m_dizzyCount = 0;
	int m_dizzyCountMax = 5;

	//==========目の追従==========//

	// 目の回転を持つ
	Math::Quaternion m_eyeQuaternion = {};

	bool  m_isEyeTracking = true;

};