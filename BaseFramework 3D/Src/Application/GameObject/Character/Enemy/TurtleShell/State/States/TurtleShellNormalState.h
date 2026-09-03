#pragma once

class TurtleShell;

#include"../TurtleShellStateBase.h"

class TurtleShellNormalState :public TurtleShellStateBase
{
public:

	void Enter(TurtleShell& turtleShell)override;
	void Update(TurtleShell& turtleShell)override;
	void Exit(TurtleShell& turtleShell)override;

};

