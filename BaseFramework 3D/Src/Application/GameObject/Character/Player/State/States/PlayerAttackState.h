#pragma once

#include"../PlayerStateBase.h"

class PlayerAttackState :public PlayerStateBase
{
public:

	PlayerAttackState(){}
	~PlayerAttackState()override{}

	void Enter(Player& player)override;
	void Update(Player& player)override;
	void Exit(Player& player)override;



};