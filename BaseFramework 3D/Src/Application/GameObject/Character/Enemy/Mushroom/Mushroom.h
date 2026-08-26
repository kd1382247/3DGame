#pragma once

#include"../EnemyBase.h"

#include"Animation/MushroomAnimation.h"
#include"State/MushroomState.h"
#include"Parameter/MushroomParameter.h"


class Mushroom :public EnemyBase
{
public:

	Mushroom() {}
	~Mushroom()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;

	void SetUpReference()override;

private:

	void UpdateMove();
	void UpdateAttack();

	void UpdateAnimation();

	void ChangeActionState(MushroomActionState  nextState);
	void ExitState(MushroomActionState _state);
	void EnterState(MushroomActionState _state);


	void UpdateActionState();

	void SetAttackTiming();

	// 攻撃判定
	void UpdateAttackCollision();


private:

	MushroomActionState m_actionState = MushroomActionState::Normal;
	MushroomMoveState   m_moveState = MushroomMoveState::Idle;

	// アニメーションクラス
	MushroomAnimation   m_animation;

	// パラメータクラス
	MushroomParameter   m_parameter;
};