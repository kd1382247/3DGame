#include "TurtleShellDamageState.h"

#include"../../TurtleShell.h"

#include"TurtleShellNormalState.h"

void TurtleShellDamageState::OnStart(TurtleShell* turtleShell)
{
	turtleShell->PlayAnimation(TurtleShellAnimationType::GetHit);
}

void TurtleShellDamageState::OnUpdate(TurtleShell * turtleShell)
{

	if (turtleShell->IsAnimationFinished())
	{
		m_pMachine->ChangeState<TurtleShellNormalState>();
	}
}

void TurtleShellDamageState::OnExit(TurtleShell * turtleShell)
{
	turtleShell->EndSpinAttack();
}
