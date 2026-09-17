#pragma once

class ChestMonster;

#include"../../../../StateMachine/StateBase.h"

class ChestMonsterDieState :public StateBase<ChestMonster>
{
public:

	void OnStart(ChestMonster* chestMonster)override;
	void OnUpdate(ChestMonster* chestMonster)override;
	void OnExit(ChestMonster* chestMonster)override;

};

