#pragma once

class TurtleShell;

#include"TurtleShellStateBase.h"

class TurtleShellStateMachine
{
public:

	void Update(TurtleShell& turtleShell);

	void ChangeState(TurtleShell& turtleShell, std::unique_ptr<TurtleShellStateBase>nextState);

private:

	std::unique_ptr<TurtleShellStateBase>m_currentState=nullptr;

};
