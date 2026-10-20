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

	// 詠唱開始から、スポナーに合図を出すまで
	float m_castTimer = 0.0f;
	bool  m_hasCast = false;

};
