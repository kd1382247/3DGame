#pragma once

#include"../EnemyBase.h"

#include"Animation/SwarmAnimation.h"
#include"State/SwarmState.h"

class Swarm :public EnemyBase
{
public:

	Swarm() {}
	~Swarm()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

private:


	void UpdateAnimation();

private:

	SwarmActionState m_actionState = SwarmActionState::Normal;
	SwarmMoveState   m_moveState = SwarmMoveState::Idle;

	// アニメーションクラス
	SwarmAnimation   m_animation;


};