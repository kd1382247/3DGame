#include "TurtleShellSpinAttackStartState.h"

#include"../../TurtleShell.h"

#include"TurtleShellSpinAttackRepeatState.h"

void TurtleShellSpinAttackStartState::Enter(TurtleShell& turtleShell)
{
	turtleShell.StartSpinAttack();
	turtleShell.PlayAnimation(TurtleShellAnimationType::SpinAttackST);
}

void TurtleShellSpinAttackStartState::Update(TurtleShell & turtleShell)
{
	if (turtleShell.IsAnimationFinished())
	{
		turtleShell.ChangeState<TurtleShellSpinAttackRepeatState>();
	}
}

void TurtleShellSpinAttackStartState::Exit(TurtleShell & turtleShell)
{

}
