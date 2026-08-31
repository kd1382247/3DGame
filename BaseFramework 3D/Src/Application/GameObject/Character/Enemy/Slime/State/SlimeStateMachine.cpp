#include "SlimeStateMachine.h"

#include"../Slime.h"

void SlimeStateMachine::Update(Slime& slime)
{
	if (!m_currentState)
	{
		return;
	}

	m_currentState->Update(slime);
}

void SlimeStateMachine::ChangeState(Slime& slime, std::unique_ptr<SlimeStateBase> nextState)
{
	if (m_currentState == nextState)
	{
		return;
	}

	if (m_currentState)
	{
		m_currentState->Exit(slime);
	}

	m_currentState = std::move(nextState);

	if (m_currentState)
	{
		m_currentState->Enter(slime);
	}

}
