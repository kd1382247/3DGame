#include "TurtleShellSpinAttackRepeatState.h"

#include"../../TurtleShell.h"

#include"TurtleShellDizzyState.h"

void TurtleShellSpinAttackRepeatState::OnStart(TurtleShell* turtleShell)
{
	turtleShell->PlayAnimation(TurtleShellAnimationType::SpinAttackRPT);
}

void TurtleShellSpinAttackRepeatState::OnUpdate(TurtleShell * turtleShell)
{

	turtleShell->UpdateSpinAttackMove();

	// 当たり判定
	turtleShell->UpdateAttackCollision();


	if (turtleShell->SpinAttackRemaining())
	{
		m_pMachine->ChangeState<TurtleShellDizzyState>();
		return;
	}
}

void TurtleShellSpinAttackRepeatState::OnExit(TurtleShell * turtleShell)
{
	turtleShell->EndSpinAttack();
}
