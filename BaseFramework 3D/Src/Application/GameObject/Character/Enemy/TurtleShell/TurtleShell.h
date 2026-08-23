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


	void UpdateMove();
	void UpdateAttack();
	void UpdateSpinAttackMove();

	bool SpinAttackRemaining();
	bool DizyyRemaining();

	void UpdateAnimation();

	void ChangeActionState(TurtleShellActionState  nextState);
	void ExitState(TurtleShellActionState _state);
	void EnterState(TurtleShellActionState _state);


	void UpdateActionState();

private:

	TurtleShellActionState m_actionState = TurtleShellActionState::Normal;
	TurtleShellMoveState   m_moveState = TurtleShellMoveState::Idle;


	float m_spinAttackDuration = 0;
	float m_spinAttackRemaining = 0;

	float m_dizzyDuration = 0;
	float m_dizzyRemaining = 0;

	Math::Vector3          m_targetPos = {};

	// アニメーションクラス
	TurtleShellAnimation   m_animation;

	// パラメータクラス
	TurtleShellParameter   m_parameter;

};