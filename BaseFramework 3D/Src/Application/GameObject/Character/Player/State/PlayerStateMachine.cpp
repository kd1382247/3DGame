#include "PlayerStateMachine.h"

#include"../Player.h"

void PlayerStateMachine::Update(Player& player)
{
	if (!m_currentState)
	{
		return;
	}

	m_currentState->Update(player);

}

void PlayerStateMachine::ChangeState(Player & player, std::unique_ptr<PlayerStateBase> nextState)
{
	if (m_currentState == nextState)
	{
		return;
	}

	// 現在Stateが存在するときだけEnter
	if(m_currentState)
	{
		m_currentState->Exit(player);
	}

	m_currentState = std::move(nextState);

	// 現在Stateが存在するときだけExit
	if (m_currentState)
	{
		m_currentState->Enter(player);
	}
}
