#pragma once

#include"../PlayerStateBase.h"

class PlayerDamageState :public PlayerStateBase
{
public:

	PlayerDamageState(){}
	~PlayerDamageState(){}

	void Enter(Player& player)override;
	void Update(Player& player)override;
	void Exit(Player& plaer)override;

};