#pragma once

class Cactas;

#include"../../../../StateMachine/StateBase.h"

class CactasNormalState :public StateBase<Cactas>
{
public:

	void OnStart(Cactas* cactas)override;
	void OnUpdate(Cactas* cactas)override;
	void OnExit(Cactas* cactas)override;

};

