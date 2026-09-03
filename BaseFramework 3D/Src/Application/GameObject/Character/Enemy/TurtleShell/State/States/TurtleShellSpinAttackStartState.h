#pragma once

class TurtleShell;

#include"../TurtleShellStateBase.h"

class TurtleShellSpinAttackStartState :public TurtleShellStateBase
{
public:

	void Enter(TurtleShell& turtleShell)override;
	void Update(TurtleShell& turtleShell)override;
	void Exit(TurtleShell& turtleShell)override;

};

