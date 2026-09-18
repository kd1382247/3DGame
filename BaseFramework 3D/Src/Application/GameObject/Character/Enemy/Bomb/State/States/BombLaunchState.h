#pragma once

class Bomb;

#include"../../../../StateMachine/StateBase.h"

class BombLaunchState :public StateBase<Bomb>
{
public:

	void OnStart(Bomb* bomb)override;
	void OnUpdate(Bomb* bomb)override;
	void OnExit(Bomb* bomb)override;

};
