#pragma once

#include"../PlayerStateBase.h"

class PlayerChargeAttackState :public PlayerStateBase
{
public:

	PlayerChargeAttackState() {}
	~PlayerChargeAttackState()override {}


	void Enter(Player& player)override;
	void Update(Player& player)override;
	void Exit(Player& player)override;

private:

	void EffectUpdate(Player& player);

	std::weak_ptr<KdEffekseerObject> m_wpEffekseerObj;

};