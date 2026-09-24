#pragma once

class Mage;

#include"../../../../StateMachine/StateBase.h"

// 2. プレイヤー位置に魔法攻撃(魔法円)
class MageTargetCircleState :public StateBase<Mage>
{
public:

	void OnStart(Mage* mage)override;
	void OnUpdate(Mage* mage)override;
	void OnExit(Mage* mage)override;

private:

	float m_castTimer = 0.0f;
	bool  m_hasCast = false;

	const int maxShot = 5;
	int       m_shotCount = 0;

};
