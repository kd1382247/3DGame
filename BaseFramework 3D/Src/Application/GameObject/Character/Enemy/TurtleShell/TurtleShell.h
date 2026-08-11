#pragma once

#include"../EnemyBase.h"

#include"Animation/TurtleShellAnimation.h"
#include"State/TurtleShellState.h"
#include"Parameter/TurtleShellParameter.h"


class TurtleShell :public EnemyBase
{
public:

	TurtleShell() {}
	~TurtleShell()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;

private:

	enum AttackState
	{
		RotationAttackST,
		RotationAttackRPT
	};

	void UpdateAnimation();

private:

	TurtleShellActionState m_actionState = TurtleShellActionState::Normal;
	TurtleShellMoveState   m_moveState = TurtleShellMoveState::Idle;

	AttackState            m_attackState = AttackState::RotationAttackST;
	// アニメーションクラス
	TurtleShellAnimation   m_animation;

	// パラメータクラス
	TurtleShellParameter   m_parameter;

};