#include "TurtleShellDizzyState.h"

#include"../../TurtleShell.h"

#include"TurtleShellNormalState.h"

void TurtleShellDizzyState::Enter(TurtleShell& turtleShell)
{
	turtleShell.StartDizzy();
	turtleShell.PlayAnimation(TurtleShellAnimationType::Dizzy);
}

void TurtleShellDizzyState::Update(TurtleShell & turtleShell)
{

	if (turtleShell.DizyyRemaining())
	{
		turtleShell.ChangeState<TurtleShellNormalState>();
		return;
	}
}

void TurtleShellDizzyState::Exit(TurtleShell & turtleShell)
{
	turtleShell.EndDizzy();
}
