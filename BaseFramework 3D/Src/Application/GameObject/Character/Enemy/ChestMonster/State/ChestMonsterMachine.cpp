#include "ChestMonsterStateMachine.h"

#include"../ChestMonster.h"

void ChestMonsterStateMachine::Update(ChestMonster& cactas)
{
	if (!m_currentState)
	{
		return;
	}

	m_currentState->Update(cactas);

}

void ChestMonsterStateMachine::ChangeState(ChestMonster & cactas, std::unique_ptr<ChestMonsterStateBase> nextState)
{
	if (m_currentState == nextState)
	{
		return;
	}

	if (m_currentState)
	{
		m_currentState->Exit(cactas);
	}

	m_currentState = std::move(nextState);

	if (m_currentState)
	{
		m_currentState->Enter(cactas);
	}

}
