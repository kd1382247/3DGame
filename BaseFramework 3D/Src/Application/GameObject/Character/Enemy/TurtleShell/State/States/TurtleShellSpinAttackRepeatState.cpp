#include "TurtleShellSpinAttackRepeatState.h"

#include"../../TurtleShell.h"

#include"TurtleShellDizzyState.h"

void TurtleShellSpinAttackRepeatState::Enter(TurtleShell& turtleShell)
{
	turtleShell.PlayAnimation(TurtleShellAnimationType::SpinAttackRPT);
}

void TurtleShellSpinAttackRepeatState::Update(TurtleShell & turtleShell)
{

	turtleShell.UpdateSpinAttackMove();

	// 当たり判定
	turtleShell.UpdateAttackCollision();


	if (turtleShell.SpinAttackRemaining())
	{
		turtleShell.ChangeState<TurtleShellDizzyState>();
		return;
	}
}

void TurtleShellSpinAttackRepeatState::Exit(TurtleShell & turtleShell)
{
	turtleShell.EndSpinAttack();
}
