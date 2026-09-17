#pragma once

class Slime;

#include"../../../../StateMachine/StateBase.h"

class SlimeDieState :public StateBase<Slime>
{
public:

	void OnStart(Slime* slime)override;
	void OnUpdate(Slime* slime)override;
	void OnExit(Slime* slime)override;
};

