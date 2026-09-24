#pragma once

class Mage;

#include"../../../../StateMachine/StateBase.h"

// 5. ボス中心から半径数メートルの範囲攻撃(HP50%以下限定)
class MageNovaCircleState :public StateBase<Mage>
{
public:

	void OnStart(Mage* mage)override;
	void OnUpdate(Mage* mage)override;
	void OnExit(Mage* mage)override;

private:

	float m_castTimer = 0.0f;
	bool  m_hasCast = false;

};
