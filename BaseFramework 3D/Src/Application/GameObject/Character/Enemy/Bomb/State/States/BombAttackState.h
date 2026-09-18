#pragma once

class Bomb;

#include"../../../../StateMachine/StateBase.h"

class BombAttackState :public StateBase<Bomb>
{
public:

	void OnStart(Bomb* bomb)override;
	void OnUpdate(Bomb* bomb)override;
	void OnExit(Bomb* bomb)override;

};
