#pragma once

#include"../EnemyBase.h"

#include"Animation/SlimeAnimation.h"
#include"State/SlimeState.h"
#include"Parameter/SlimeParameter.h"

class Slime :public EnemyBase
{
public:

	Slime() {}
	~Slime()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;

private:


	void UpdateMove();
	void UpdateAttack();

	void UpdateActionState();

	void UpdateAnimation();

	void ChangeActionState(SlimeActionState  nextState);
	void ExitState(SlimeActionState _state);
	void EnterState(SlimeActionState _state);


	
private:

	SlimeActionState m_actionState = SlimeActionState::Normal;
	SlimeMoveState   m_moveState = SlimeMoveState::Idle;

	// アニメーションクラス
	SlimeAnimation   m_animation;

	// パラメータクラス
	SlimeParameter   m_parameter;

};