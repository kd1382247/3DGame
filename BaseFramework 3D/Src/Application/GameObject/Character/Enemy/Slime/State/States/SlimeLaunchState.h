#pragma once

class Slime;

#include"../../../../StateMachine/StateBase.h"

class SlimeLaunchState :public StateBase<Slime>
{
public:

	void OnStart(Slime* slime)override;
	void OnUpdate(Slime* slime)override;
	void OnExit(Slime* slime)override;
};

