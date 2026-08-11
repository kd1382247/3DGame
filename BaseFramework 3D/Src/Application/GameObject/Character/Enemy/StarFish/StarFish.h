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
