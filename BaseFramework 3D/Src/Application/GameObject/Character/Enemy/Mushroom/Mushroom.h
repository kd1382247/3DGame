#pragma once

#include"../EnemyBase.h"

#include"Animation/MushroomAnimationType.h"
#include"Animation/MushroomAnimation.h"
#include"Parameter/MushroomParameter.h"
#include"State/MushroomStateMachine.h"


class Mushroom :public EnemyBase
{
public:

	Mushroom() {}
	~Mushroom()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;

	void DrawDebug()override;

	void SetUpReference()override;
	
	template<class T>
	void ChangeState()
	{
		m_stateMachine.ChangeState(*this, std::make_unique<T>());
	}

	void UpdateMove();

	bool IsAttack()const { return m_attackFlg; }
	bool IsLaunch()const { return m_launchFlg; }
	

	// パラメータのゲッター
	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }
	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }
	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

	void PlayAnimation(MushroomAnimationType type);
	void RePlayAnimation(MushroomAnimationType type);

	bool IsAnimationFinished()const { return m_animation.IsFinished(); }


	void StartAttack();
	void EndAttack();

	void UpdateLaunch();

	void OnHit(const AttackInfo attackInfo)override;

	// 攻撃判定
	void UpdateAttackCollision();

private:

	void UpdateAttack();

	void UpdateAnimation();

	void SetAttackTiming();

private:

	// アニメーションクラス
	MushroomAnimation   m_animation;

	// パラメータクラス
	MushroomParameter   m_parameter;

	// ステートマシン
	MushroomStateMachine m_stateMachine;

};