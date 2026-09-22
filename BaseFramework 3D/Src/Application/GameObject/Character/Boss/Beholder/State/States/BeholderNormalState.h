#pragma once

class Beholder;

#include"../../../../StateMachine/StateBase.h"

class BeholderNormalState :public StateBase<Beholder>
{
public:

	void OnStart(Beholder* beholder)override;
	void OnUpdate(Beholder* beholder)override;
	void OnExit(Beholder* beholder)override;

};
