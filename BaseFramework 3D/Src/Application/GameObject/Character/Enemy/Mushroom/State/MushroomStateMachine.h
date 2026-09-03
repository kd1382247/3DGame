#pragma once

class Mushroom;

#include"MushroomStateBase.h"

class MushroomStateMachine
{
public:

	void Update(Mushroom& mushroom);

	void ChangeState(Mushroom& mushroom, std::unique_ptr<MushroomStateBase>nextState);

private:

	std::unique_ptr<MushroomStateBase>m_currentState=nullptr;

};
