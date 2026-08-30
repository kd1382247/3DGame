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

	void DrawDebug()override;

	void SetUpReference()override;


	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }
	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }

	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

private:

	void UpdateLaunch();


	void UpdateMove();
	void UpdateAttack();
	void UpdateSpinAttackMove();

	bool SpinAttackRemaining();
	bool DizyyRemaining();
	void HitCoolDownRemaining();


	void UpdateAnimation();

	void ChangeActionState(TurtleShellActionState  nextState);
	void ExitState(TurtleShellActionState _state);
	void EnterState(TurtleShellActionState _state);


	void UpdateActionState();

	// 攻撃判定
	void UpdateAttackCollision();

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


	float m_hitCooldownDuration = 0.0f;
	float m_hitCooldownRemaining = 0.0f;

};