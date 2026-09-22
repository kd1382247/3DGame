#pragma once

class Golem;

#include"../../../../StateMachine/StateBase.h"

class GolemNormalState :public StateBase<Golem>
{
public:

	void OnStart(Golem* golem)override;
	void OnUpdate(Golem* golem)override;
	void OnExit(Golem* golem)override;

};
