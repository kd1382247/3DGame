#pragma once

#include"../PlayerStateBase.h"

class PlayerNormalState :public PlayerStateBase
{
public:

	PlayerNormalState(){}
	~PlayerNormalState()override{}


	void Enter(Player& player)override;
	void Update(Player& player)override;
	void Exit(Player& player)override;

};