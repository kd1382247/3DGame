#include "TurtleShellLaunchState.h"

#include"../../TurtleShell.h"

#include"TurtleShellNormalState.h"

void TurtleShellLaunchState::Enter(TurtleShell& turtleShell)
{
	turtleShell.PlayAnimation(TurtleShellAnimationType::Idle);
}

void TurtleShellLaunchState::Update(TurtleShell & turtleShell)
{
	turtleShell.UpdateLaunch();

	if (!turtleShell.IsLaunch())
	{
		turtleShell.ChangeState<TurtleShellNormalState>();
	}
}

void TurtleShellLaunchState::Exit(TurtleShell & turtleShell)
{

}
