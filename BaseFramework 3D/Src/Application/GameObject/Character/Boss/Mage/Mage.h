#pragma once

#include"../BossBase.h"

#include"Animation/MageAnimation.h"
#include"Parameter/MageParameter.h"
#include"../../StateMachine/StateMachine.h"


class Mage :public BossBase
{
public:

	Mage() {}
	~Mage()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }

	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }

	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

	void PlayAnimation(MageAnimationType type);

private:


	void UpdateAnimation();

	void DrawParameterInspector() override;

private:

	// アニメーションクラス
	MageAnimation   m_animation;

	// パラメータクラス
	MageParameter   m_parameter;

	// ステートマシン
	StateMachine<Mage> m_stateMachine;

};