#pragma once

class ChestMonster;

class ChestMonsterStateBase
{
public:

	ChestMonsterStateBase(){}
	virtual ~ChestMonsterStateBase(){}

	virtual void Enter(ChestMonster&chestMonster){}
	virtual void Update(ChestMonster& chestMonster) = 0;
	virtual void Exit(ChestMonster&chestMonster){}

};