#pragma once

class Player;

#include"PlayerStateBase.h"

class PlayerStateMachine
{
public:

	void Update(Player& player);

	void ChangeState(Player& player, std::unique_ptr<PlayerStateBase>nextState);



private:

	std::unique_ptr<PlayerStateBase>m_currentState=nullptr;

};
