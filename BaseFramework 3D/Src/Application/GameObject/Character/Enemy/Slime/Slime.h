#pragma once

#include"../EnemyBase.h"

#include"Animation/SlimeAnimation.h"
#include"State/SlimeState.h"

class Slime :public EnemyBase
{
public:

	Slime() {}
	~Slime()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

private:


	void UpdateAnimation();

private:

	SlimeActionState m_actionState = SlimeActionState::Normal;
	SlimeMoveState   m_moveState = SlimeMoveState::Idle;

	// アニメーションクラス
	SlimeAnimation   m_animation;


};