#include "MushroomStateMachine.h"

#include"../Mushroom.h"

void MushroomStateMachine::Update(Mushroom& mushroom)
{
	if (!m_currentState)
	{
		return;
	}

	m_currentState->Update(mushroom);

}

void MushroomStateMachine::ChangeState(Mushroom & mushroom, std::unique_ptr<MushroomStateBase> nextState)
{
	if (m_currentState == nextState)
	{
		return;
	}

	if (m_currentState)
	{
		m_currentState->Exit(mushroom);
	}

	m_currentState = std::move(nextState);

	if (m_currentState)
	{
		m_currentState->Enter(mushroom);
	}

}
