#pragma once

#include"../EnemyBase.h"

#include"Animation/CactasAnimation.h"
#include"State/CactasState.h"
#include"Parameter/CactasParameter.h"

class Cactas :public EnemyBase
{
public:

	Cactas() {}
	~Cactas()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;


private:

	void UpdateMove();
	void UpdateAttack();

	void UpdateAnimation();

	void ChangeActionState(CactasActionState  nextState);
	void ExitState(CactasActionState _state);
	void EnterState(CactasActionState _state);

	void UpdateActionState();

	void SetAttackTiming();


	// 攻撃判定
	void UpdateAttackCollision();


private:

	CactasActionState m_actionState = CactasActionState::Normal;
	CactasMoveState   m_moveState = CactasMoveState::Idle;

	// アニメーションクラス
	CactasAnimation   m_animation;

	// パラメータクラス
	CactasParameter   m_parameter;




};