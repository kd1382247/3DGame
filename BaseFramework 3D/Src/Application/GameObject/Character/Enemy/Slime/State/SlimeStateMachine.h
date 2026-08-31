#pragma once

class Slime;

#include"SlimeStateBase.h"

class SlimeStateMachine
{
public:

	void Update(Slime& slime);

	void ChangeState(Slime& slime, std::unique_ptr<SlimeStateBase>nextState);

private:

	std::unique_ptr<SlimeStateBase>m_currentState = nullptr;

};
