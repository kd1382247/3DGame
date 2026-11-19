#pragma once

#include"../../../../StateMachine/StateBase.h"

#include"../../Beholder.h"

class BeholderSparkAttackState :public StateBase<Beholder>
{

public:

	void OnStart(Beholder* owner)override;
	void OnUpdate(Beholder* owner)override;
	void OnExit(Beholder* owner)override;

private:

	bool m_startFire=false;

};