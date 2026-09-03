#include "TurtleShellDieState.h"

#include"../../TurtleShell.h"

void TurtleShellDieState::Enter(TurtleShell& turtleShell)
{
	turtleShell.PlayAnimation(TurtleShellAnimationType::Die);
}

void TurtleShellDieState::Update(TurtleShell & turtleShell)
{
	if (turtleShell.IsAnimationFinished())
	{
		turtleShell.Destroy();
	}
}

void TurtleShellDieState::Exit(TurtleShell & turtleShell)
{

}
