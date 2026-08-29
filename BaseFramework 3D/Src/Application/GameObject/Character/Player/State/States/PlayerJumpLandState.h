#pragma once

#include"../PlayerStateBase.h"

class PlayerJumpLandState :public PlayerStateBase
{
public:

	PlayerJumpLandState() {}
	~PlayerJumpLandState()override{}

	void Enter(Player& player)override;
	void Update(Player& player)override;
	void Exit(Player& player)override;

};