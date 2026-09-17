#pragma once

class Mushroom;

#include"../../../../StateMachine/StateBase.h"

class MushroomLaunchState :public StateBase<Mushroom>
{
public:

	void OnStart(Mushroom* mushroom)override;
	void OnUpdate(Mushroom* mushroom)override;
	void OnExit(Mushroom* mushroom)override;

};

