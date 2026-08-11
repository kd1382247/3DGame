#pragma once

#include"../EnemyBase.h"

#include"Animation/MushroomAnimation.h"
#include"State/MushroomState.h"
#include"Parameter/MushroomParameter.h"


class Mushroom :public EnemyBase
{
public:

	Mushroom() {}
	~Mushroom()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;

private:


	void UpdateAnimation();

private:

	MushroomActionState m_actionState = MushroomActionState::Normal;
	MushroomMoveState   m_moveState = MushroomMoveState::Idle;

	// アニメーションクラス
	MushroomAnimation   m_animation;

	// パラメータクラス
	MushroomParameter   m_parameter;

};