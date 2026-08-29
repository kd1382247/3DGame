#pragma once

#include"../PlayerStateBase.h"

class PlayerJumpStartState :public PlayerStateBase
{
public:

	PlayerJumpStartState(){}
	~PlayerJumpStartState()override {}


	void Enter(Player& player)override;
	void Update(Player& player)override;
	void Exit(Player& player)override;

};