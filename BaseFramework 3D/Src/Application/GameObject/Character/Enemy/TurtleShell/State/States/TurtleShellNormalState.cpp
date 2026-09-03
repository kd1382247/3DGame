#include "TurtleShellNormalState.h"

#include"../../TurtleShell.h"

#include"TurtleShellSpinAttackStartState.h"
#include"TurtleShellLaunchState.h"

void TurtleShellNormalState::Enter(TurtleShell& turtleShell)
{
	turtleShell.PlayAnimation(TurtleShellAnimationType::Idle);
}

void TurtleShellNormalState::Update(TurtleShell & turtleShell)
{

	turtleShell.UpdateMove();

	if (turtleShell.IsLaunch())
	{
		turtleShell.ChangeState<TurtleShellLaunchState>();
		return;
	}

	if (turtleShell.IsAttack())
	{
		turtleShell.ChangeState<TurtleShellSpinAttackStartState>();
		return;
	}

}

void TurtleShellNormalState::Exit(TurtleShell & turtleShell)
{

}
