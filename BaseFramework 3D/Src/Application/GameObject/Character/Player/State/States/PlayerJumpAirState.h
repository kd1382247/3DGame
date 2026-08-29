#pragma once

#include"../PlayerStateBase.h"

class PlayerJumpAirState :public PlayerStateBase
{
public:

	PlayerJumpAirState(){}
	~PlayerJumpAirState()override{}

	void Enter(Player& player)override;
	void Update(Player& player)override;
	void Exit(Player& player)override;

};