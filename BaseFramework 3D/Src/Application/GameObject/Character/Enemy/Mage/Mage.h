#pragma once

#include"../EnemyBase.h"

#include"Animation/MageAnimation.h"
#include"State/MageState.h"
#include"Parameter/MageParameter.h"


class Mage :public EnemyBase
{
public:

	Mage() {}
	~Mage()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspecter()override;

private:

	enum class AttackState
	{
		Attack1,
		Attack2
	};

	void UpdateAnimation();

private:

	MageActionState m_actionState = MageActionState::Normal;
	MageMoveState   m_moveState = MageMoveState::Idle;

	AttackState     m_attackState = AttackState::Attack1;

	// アニメーションクラス
	MageAnimation   m_animation;

	// パラメータクラス
	MageParameter   m_parameter;

};