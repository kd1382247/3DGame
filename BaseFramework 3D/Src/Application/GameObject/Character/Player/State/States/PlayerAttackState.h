#pragma once

class Player;

#include"../../../StateMachine/StateBase.h"

class PlayerAttackState :public StateBase<Player>
{
public:

	void OnStart(Player* owner)override;
	void OnUpdate(Player* owner)override;
	void OnExit(Player* owner)override;

};