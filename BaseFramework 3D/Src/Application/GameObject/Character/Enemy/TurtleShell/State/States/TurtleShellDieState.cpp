#include "TurtleShellDieState.h"

#include"../../TurtleShell.h"

void TurtleShellDieState::OnStart(TurtleShell* turtleShell)
{
	turtleShell->PlayAnimation(TurtleShellAnimationType::Die);
}

void TurtleShellDieState::OnUpdate(TurtleShell * turtleShell)
{
	if (turtleShell->IsAnimationFinished())
	{
		turtleShell->Destroy();
	}
}

void TurtleShellDieState::OnExit(TurtleShell * turtleShell)
{

}
