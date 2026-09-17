#pragma once

class Player;

#include"../../../StateMachine/StateBase.h"

class PlayerChargeAttackState :public StateBase<Player>
{
public:

	void OnStart(Player* owner)override;
	void OnUpdate(Player* owner)override;
	void OnExit(Player* owner)override;

private:

	void EffectUpdate(Player* owner);

	std::weak_ptr<KdEffekseerObject> m_wpEffekseerObj;

};