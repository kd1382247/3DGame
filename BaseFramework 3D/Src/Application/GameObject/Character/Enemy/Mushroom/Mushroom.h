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

	void DrawDebug()override;

	void SetUpReference()override;
	
	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }

	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }

	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

private:

	void UpdateLaunch();


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