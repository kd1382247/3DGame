#pragma once

class Player;

#include"../../../StateMachine/StateBase.h"

class PlayerSpecialMoveState :public StateBase<Player>
{
public:

	void OnStart(Player* owner)override;
	void OnUpdate(Player* owner)override;
	void OnExit(Player* owner)override;

};