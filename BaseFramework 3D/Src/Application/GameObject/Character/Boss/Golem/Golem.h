#pragma once

#include"../BossBase.h"

#include"Animation/GolemAnimation.h"
#include"Parameter/GolemParameter.h"
#include"../../StateMachine/StateMachine.h"


class Golem :public BossBase
{
public:

	Golem() {}
	~Golem()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }

	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }

	float GetMoveSpeed()const { return m_parameter.GetParam().m_moveSpeed; }

	void PlayAnimation(GolemAnimationType type);

private:

	void UpdateAnimation();

	void DrawParameterInspector() override;

private:


	// アニメーションクラス
	GolemAnimation   m_animation;

	// パラメータクラス
	GolemParameter   m_parameter;

	// ステートマシン
	StateMachine<Golem> m_stateMachine;

};