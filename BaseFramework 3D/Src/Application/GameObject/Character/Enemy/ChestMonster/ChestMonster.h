#pragma once

#include"../EnemyBase.h"

#include"Animation/ChestMonsterAnimation.h"
#include"State/ChestMonsterState.h"

class ChestMonster :public EnemyBase
{
public:

	ChestMonster() {}
	~ChestMonster()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

private:


	void UpdateAnimation();

private:

	ChestMonsterActionState m_actionState = ChestMonsterActionState::Normal;
	ChestMonsterMoveState   m_moveState = ChestMonsterMoveState::Idle;

	// アニメーションクラス
	ChestMonsterAnimation   m_animation;


};