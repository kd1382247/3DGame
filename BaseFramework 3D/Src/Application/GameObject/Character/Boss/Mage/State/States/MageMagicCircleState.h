#pragma once

class Mage;

#include"../../../../StateMachine/StateBase.h"

class MageMagicCircleState :public StateBase<Mage>
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
