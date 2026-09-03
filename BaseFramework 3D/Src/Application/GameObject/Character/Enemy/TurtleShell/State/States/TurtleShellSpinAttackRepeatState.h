#pragma once

class TurtleShell;

#include"../TurtleShellStateBase.h"

class TurtleShellSpinAttackRepeatState :public TurtleShellStateBase
{
public:

	void Enter(TurtleShell& turtleShell)override;
	void Update(TurtleShell& turtleShell)override;
	void Exit(TurtleShell& turtleShell)override;

};

