#pragma once

class ChestMonster;

class ChestMonsterStateBase
{
public:

	ChestMonsterStateBase(){}
	virtual ~ChestMonsterStateBase(){}

	virtual void Enter(ChestMonster&cactas){}
	virtual void Update(ChestMonster& cactas) = 0;
	virtual void Exit(ChestMonster&cactas){}

};