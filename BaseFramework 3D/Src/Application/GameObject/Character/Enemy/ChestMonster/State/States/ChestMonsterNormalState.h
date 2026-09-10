#pragma once

class ChestMonster;

#include"../ChestMonsterStateBase.h"

class ChestMonsterNormalState :public ChestMonsterStateBase
{
public:

	void Enter(ChestMonster& chestMonster)override;
	void Update(ChestMonster& chestMonster)override;
	void Exit(ChestMonster& chestMonster)override;

};

