#pragma once

#include"../PlayerStateBase.h"

class PlayerGuardState :public PlayerStateBase
{
public:

	PlayerGuardState() {}
	~PlayerGuardState()override {}

	void Enter(Player& player)override;
	void Update(Player& player)override;
	void Exit(Player& player)override;

};