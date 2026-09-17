#pragma once

class TurtleShell;

#include"../../../../StateMachine/StateBase.h"

class TurtleShellDamageState :public StateBase<TurtleShell>
{
public:

	void OnStart(TurtleShell* turtleShell)override;
	void OnUpdate(TurtleShell* turtleShell)override;
	void OnExit(TurtleShell* turtleShell)override;

};

