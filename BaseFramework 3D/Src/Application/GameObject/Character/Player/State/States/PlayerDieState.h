#pragma once

#include"../PlayerStateBase.h"

class PlayerDieState :public PlayerStateBase
{
public:

	PlayerDieState() {}
	~PlayerDieState()override {}

	void Enter(Player& player)override;
	void Update(Player& player)override;
	void Exit(Player& player)override;

};