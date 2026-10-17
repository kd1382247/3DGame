#pragma once

class Mage;

#include"../../../../StateMachine/StateBase.h"

class MageIntroState :public StateBase<Mage>
{
public:


	void OnStart(Mage* mage)override;
	void OnUpdate(Mage* mage)override;
	void OnExit(Mage* mage)override;

private:

	enum class Phase
	{
		WaitStart,
		WaitCamera,
		Roar,
		WaitEnd
	};

	Phase m_introPhase = Phase::WaitStart;

	bool m_isRoar = false;
};
