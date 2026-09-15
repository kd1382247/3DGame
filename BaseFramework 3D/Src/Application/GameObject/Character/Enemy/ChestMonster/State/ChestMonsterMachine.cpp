#include "ChestMonsterStateMachine.h"

#include"../ChestMonster.h"

void ChestMonsterStateMachine::Update(ChestMonster& chestMonster)
{
	if (!m_currentState)
	{
		return;
	}

	m_currentState->Update(chestMonster);

}

void ChestMonsterStateMachine::ChangeState(ChestMonster & chestMonster, std::unique_ptr<ChestMonsterStateBase> nextState)
{
	if (m_currentState == nextState)
	{
		return;
	}

	if (m_currentState)
	{
		m_currentState->Exit(chestMonster);
	}

	m_currentState = std::move(nextState);

	if (m_currentState)
	{
		m_currentState->Enter(chestMonster);
	}

}
