#pragma once

#include"../EnemyBase.h"

#include"Animation/StarFishAnimation.h"
#include"State/StarFishState.h"
#include"Parameter/StarFishParameter.h"


class StarFish :public EnemyBase
{
public:

	StarFish() {}
	~StarFish()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;

	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }

	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }

	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

private:


	void UpdateAnimation();

private:

	StarFishActionState m_actionState = StarFishActionState::Normal;
	StarFishMoveState   m_moveState = StarFishMoveState::Idle;

	// アニメーションクラス
	StarFishAnimation   m_animation;

	// パラメータクラス
	StarFishParameter   m_parameter;

};
