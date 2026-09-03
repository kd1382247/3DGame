#include "TurtleShellDamageState.h"

#include"../../TurtleShell.h"

#include"TurtleShellNormalState.h"

void TurtleShellDamageState::Enter(TurtleShell& turtleShell)
{
	turtleShell.PlayAnimation(TurtleShellAnimationType::GetHit);
}

void TurtleShellDamageState::Update(TurtleShell & turtleShell)
{

	if (turtleShell.IsAnimationFinished())
	{
		turtleShell.ChangeState<TurtleShellNormalState>();
	}
}

void TurtleShellDamageState::Exit(TurtleShell & turtleShell)
{
	turtleShell.EndSpinAttack();
}
