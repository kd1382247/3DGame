#pragma once

#include"../EnemyBase.h"

#include"Animation/BeholderAnimation.h"
#include"State/BeholderState.h"

class Beholder :public EnemyBase
{
public:

	Beholder(){}
	~Beholder()override{}

	void Init()override;
	void Update()override;
	void PostUpdate()override;



private:


	void UpdateAnimation();

private:

	BeholderActionState m_actionState=BeholderActionState::Normal;
	BeholderMoveState   m_moveState=BeholderMoveState::Idle;

	// アニメーションクラス
	BeholderAnimation   m_animation;


};