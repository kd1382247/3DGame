#pragma once

class Slime;

#include"../SlimeStateBase.h"

class SlimeLaunchState :public SlimeStateBase
{
public:

	void Enter(Slime& slime)override;
	void Update(Slime& slime)override;
	void Exit(Slime& slime)override;
};

