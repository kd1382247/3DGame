#pragma once

#include"../EnemyBase.h"

#include"Animation/BombAnimation.h"
#include"State/BombState.h"

class Bomb :public EnemyBase
{
public:

	Bomb() {}
	~Bomb()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

private:


	void UpdateAnimation();

private:

	BombActionState m_actionState = BombActionState::Normal;
	BombMoveState   m_moveState = BombMoveState::Idle;

	// アニメーションクラス
	BombAnimation   m_animation;


};