#include "CactasStateMachine.h"

#include"../Cactas.h"

void CactasStateMachine::Update(Cactas& cactas)
{
	if (!m_currentState)
	{
		return;
	}

	m_currentState->Update(cactas);

}

void CactasStateMachine::ChangeState(Cactas & cactas, std::unique_ptr<CactasStateBase> nextState)
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
