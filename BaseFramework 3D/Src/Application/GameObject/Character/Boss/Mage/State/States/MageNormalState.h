#pragma once

class Mage;

#include"../../../../StateMachine/StateBase.h"

class MageNormalState :public StateBase<Mage>
{
public:

	void OnStart(Mage* mage)override;
	void OnUpdate(Mage* mage)override;
	void OnExit(Mage* mage)override;

};
