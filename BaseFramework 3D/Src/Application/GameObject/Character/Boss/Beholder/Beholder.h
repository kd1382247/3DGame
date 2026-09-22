#pragma once

#include"../BossBase.h"

#include"Animation/BeholderAnimation.h"
#include"Parameter/BeholderParameter.h"
#include"../../StateMachine/StateMachine.h"


class Beholder :public BossBase
{
public:

	Beholder(){}
	~Beholder()override{}

	void Init()override;
	void Update()override;
	void PostUpdate()override;


	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }
	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }
	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

	void PlayAnimation(BeholderAnimationType type);

private:


	void DrawParameterInspector()override;

	void UpdateAnimation();

private:


	// アニメーションクラス
	BeholderAnimation   m_animation;
	// パラメータークラス
	BeholderParameter   m_parameter;

	// ステートマシン
	StateMachine<Beholder> m_stateMachine;


};