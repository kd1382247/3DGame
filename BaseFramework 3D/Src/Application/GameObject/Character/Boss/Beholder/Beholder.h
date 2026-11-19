#pragma once

#include"../BossBase.h"

#include"Animation/BeholderAnimation.h"
#include"Parameter/BeholderParameter.h"
#include"../../StateMachine/StateMachine.h"


#include"SparkAttack/BeholderSparkAttack.h"


class Beholder :public BossBase
{
public:

	Beholder(){}
	~Beholder()override{}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }
	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

	void PlayAnimation(BeholderAnimationType type);


	// SparkBullet関連
	void CreateSparkBullet();
	void FireSparkBullet();
	bool IsStandbySparkBullet();


	// 目をプレイヤーに追従させるかをセット
	void SetEyeTracking(bool flg) { m_isEyeTracking = flg; }



	bool IsAnimationFinished()const { return m_animation.IsFinished(); }
	float AnimProgress()      const { return m_animation.AnimProgress(); }

	bool IsAttack()const { return m_attackFlg; }


	void UpdateAttack();
	void EndAttack();


private:



	void PlayWalkAnimation()override;
	void PlayIdleAnimation()override;

	void DrawParameterInspector()override;

	void UpdateAnimation();

	Math::Quaternion CalcEyeTargetRotation(const Math::Matrix& eyeWorldMat);

	// モデルの目の回転(X軸)
	void UpdateEyeRotationX();

private:

	// 攻撃クラス(SparkBullet)
	BeholderSparkAttack m_sparkAttack;


	// アニメーションクラス
	BeholderAnimation   m_animation;
	// パラメータークラス
	BeholderParameter   m_parameter;

	// ステートマシン
	StateMachine<Beholder> m_stateMachine;


	// 目の回転を持つ
	Math::Quaternion m_eyeQuaternion = {};

	bool  m_isEyeTracking = true;

};