#pragma once

#include"../EnemyBase.h"

#include"Animation/CactasAnimation.h"
#include"State/CactasState.h"

class Cactas :public EnemyBase
{
public:

	Cactas() {}
	~Cactas()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

private:


	void UpdateAnimation();

private:

	CactasActionState m_actionState = CactasActionState::Normal;
	CactasMoveState   m_moveState = CactasMoveState::Idle;

	// アニメーションクラス
	CactasAnimation   m_animation;


};