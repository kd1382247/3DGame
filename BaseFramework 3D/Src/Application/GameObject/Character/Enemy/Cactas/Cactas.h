#pragma once

#include"../EnemyBase.h"

#include"Animation/CactasAnimationType.h"
#include"Animation/CactasAnimation.h"
#include"State/CactasState.h"
#include"Parameter/CactasParameter.h"

#include"State/CactasStateMachine.h"

class Cactas :public EnemyBase
{
public:

	Cactas() {}
	~Cactas()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;

	void SetUpReference()override;

	void DrawDebug()override;

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

	void PlayAnimation(CactasAnimationType type);

	bool IsAnimationFinished()const { return m_animation.IsFinished(); }

	void StartAttack();
	void EndAttack();

	void UpdateLaunch();

	// 攻撃判定
	void UpdateAttackCollision();

	void OnHit(const AttackInfo attackInfo) override;

private:

	
	void OutroUpdate();

	void UpdateAttack();

	void UpdateAnimation();

	void SetAttackTiming();



private:

	CactasActionState m_actionState = CactasActionState::Normal;
	CactasMoveState   m_moveState = CactasMoveState::Idle;

	// アニメーションクラス
	CactasAnimation   m_animation;

	// パラメータクラス
	CactasParameter   m_parameter;

	// ステートマシン
	CactasStateMachine m_stateMachine;
};