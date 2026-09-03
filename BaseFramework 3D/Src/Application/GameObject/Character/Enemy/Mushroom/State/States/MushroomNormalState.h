#pragma once

class Mushroom;

#include"../MushroomStateBase.h"

class MushroomNormalState :public MushroomStateBase
{
public:

	void Enter(Mushroom& mushroom)override;
	void Update(Mushroom& mushroom)override;
	void Exit(Mushroom& mushroom)override;

};

