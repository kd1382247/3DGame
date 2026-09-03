#pragma once

class TurtleShell;

class TurtleShellStateBase
{
public:

	TurtleShellStateBase(){}
	virtual ~TurtleShellStateBase(){}

	virtual void Enter(TurtleShell&turtleShell){}
	virtual void Update(TurtleShell& turtleShell) = 0;
	virtual void Exit(TurtleShell&turtleShell){}

};