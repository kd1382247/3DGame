#pragma once

class Cactas;

#include"CactasStateBase.h"

class CactasStateMachine
{
public:

	void Update(Cactas& cactas);

	void ChangeState(Cactas& cactas, std::unique_ptr<CactasStateBase>nextState);

private:

	std::unique_ptr<CactasStateBase>m_currentState=nullptr;

};
