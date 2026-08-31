#pragma once

class Cactas;

#include"../CactasStateBase.h"

class CactasLaunchState :public CactasStateBase
{
public:

	void Enter(Cactas& cactas)override;
	void Update(Cactas& cactas)override;
	void Exit(Cactas& cactas)override;

};

