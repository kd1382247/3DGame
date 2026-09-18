#pragma once

#include"../EnemyBase.h"

#include"Animation/StarFishAnimation.h"
#include"State/StarFishState.h"
#include"Parameter/StarFishParameter.h"

#include"../../StateMachine/StateMachine.h"


class StarFish :public EnemyBase
{
public:

	StarFish() {}
	~StarFish()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void SetUpReference()override;

	bool IsAttack()const { return m_attackFlg; }
	bool IsLaunch()const { return m_launchFlg; }

	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }

	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }

	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

	void PlayAnimation(StarFishAnimationType type);
	void RePlayAnimation(StarFishAnimationType type);

	bool IsAnimationFinished()const { return m_animation.IsFinished(); }

	void StartAttack();
	void EndAttack();

	void UpdateLaunch();

	void OnHit(const AttackInfo attackInfo)override;

	// 攻撃判定
	void UpdateAttackCollision();

private:

	void UpdateAnimation();

	void PlayWalkAnimation() override;
	void PlayIdleAnimation() override;

	void DrawParameterInspector() override;

	void SetAttackTiming();

private:

	// アニメーションクラス
	StarFishAnimation   m_animation;

	// パラメータクラス
	StarFishParameter   m_parameter;

	// ステートマシン
	StateMachine<StarFish> m_stateMachine;

};
