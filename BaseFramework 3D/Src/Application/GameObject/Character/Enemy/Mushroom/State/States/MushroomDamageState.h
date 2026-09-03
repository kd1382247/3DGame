#pragma once

class Mushroom;

#include"../MushroomStateBase.h"

class MushroomDamageState :public MushroomStateBase
{
public:

	void Enter(Mushroom& mushroom)override;
	void Update(Mushroom& mushroom)override;
	void Exit(Mushroom& mushroom)override;

};

