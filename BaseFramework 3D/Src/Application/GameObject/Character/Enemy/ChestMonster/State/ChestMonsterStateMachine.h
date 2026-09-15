#pragma once

class ChestMonster;

#include"ChestMonsterStateBase.h"

class ChestMonsterStateMachine
{
public:

	void Update(ChestMonster& chestMonster);

	void ChangeState(ChestMonster& chestMonster, std::unique_ptr<ChestMonsterStateBase>nextState);

private:

	std::unique_ptr<ChestMonsterStateBase>m_currentState=nullptr;

};
