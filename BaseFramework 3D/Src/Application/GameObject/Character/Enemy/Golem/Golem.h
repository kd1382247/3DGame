#pragma once

#include"../EnemyBase.h"

#include"Animation/GolemAnimation.h"
#include"State/GolemState.h"

class Golem :public EnemyBase
{
public:

	Golem() {}
	~Golem()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

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


};