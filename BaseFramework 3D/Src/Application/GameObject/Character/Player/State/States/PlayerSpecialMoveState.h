#pragma once

#include"../PlayerStateBase.h"

class PlayerSpecialMoveState :public PlayerStateBase
{
public:

	PlayerSpecialMoveState() {}
	~PlayerSpecialMoveState()override {}


	void Enter(Player& player)override;
	void Update(Player& player)override;
	void Exit(Player& player)override;

};