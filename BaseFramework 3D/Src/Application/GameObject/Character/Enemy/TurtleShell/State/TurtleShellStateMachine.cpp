#include "TurtleShellStateMachine.h"

#include"../TurtleShell.h"

void TurtleShellStateMachine::Update(TurtleShell& turtleShell)
{
	if (!m_currentState)
	{
		return;
	}

	m_currentState->Update(turtleShell);

}

void TurtleShellStateMachine::ChangeState(TurtleShell & turtleShell, std::unique_ptr<TurtleShellStateBase> nextState)
{
	if (m_currentState == nextState)
	{
		return;
	}

	if (m_currentState)
	{
		m_currentState->Exit(turtleShell);
	}

	m_currentState = std::move(nextState);

	if (m_currentState)
	{
		m_currentState->Enter(turtleShell);
	}

}
