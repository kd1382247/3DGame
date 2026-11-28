#pragma once

class Bomb;

#include"../../../../StateMachine/StateBase.h"

class BombChargeState :public StateBase<Bomb>
{
public:

	void OnStart(Bomb* bomb)override;
	void OnUpdate(Bomb* bomb)override;
	void OnExit(Bomb* bomb)override;

private:

	void UpdateEffects(const Math::Vector3& pos);

	std::vector<std::weak_ptr<KdEffekseerObject>>m_effectObjects;
};
