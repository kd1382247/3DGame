#pragma once

class Mage;

#include"../../../../StateMachine/StateBase.h"

class MageSummonState :public StateBase<Mage>
{
public:

	void OnStart(Mage* mage)override;
	void OnUpdate(Mage* mage)override;
	void OnExit(Mage* mage)override;

private:

	// 詠唱開始〜煙演出が始まるまで
	float m_castTimer = 0.0f;
	bool  m_hasCast = false;

	// 煙演出が始まってから、実際に敵が出現するまで
	float m_summonTimer = 0.0f;
	bool  m_hasSpawned = false;

};
