#pragma once

#include"../EnemyBase.h"

#include"Animation/GolemAnimation.h"
#include"State/GolemState.h"
#include"Parameter/GolemParameter.h"


class Golem :public EnemyBase
{
public:

	Golem() {}
	~Golem()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;

	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }

	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }

	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

private:

	enum class AttackState
	{
		Attack1,
		Attack2
	};

	void UpdateAnimation();

private:

	GolemActionState m_actionState = GolemActionState::Normal;
	GolemMoveState   m_moveState = GolemMoveState::Idle;

	AttackState      m_attackState = AttackState::Attack1;

	// アニメーションクラス
	GolemAnimation   m_animation;

	// パラメータクラス
	GolemParameter   m_parameter;

};