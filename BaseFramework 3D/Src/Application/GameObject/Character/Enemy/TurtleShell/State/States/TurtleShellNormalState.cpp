#include "TurtleShellNormalState.h"

#include"../../TurtleShell.h"

#include"TurtleShellSpinAttackStartState.h"
#include"TurtleShellLaunchState.h"

void TurtleShellNormalState::OnStart(TurtleShell* turtleShell)
{
	turtleShell->PlayAnimation(TurtleShellAnimationType::Idle);
}

void TurtleShellNormalState::OnUpdate(TurtleShell * turtleShell)
{

	turtleShell->UpdateMove();

	if (turtleShell->IsLaunch())
	{
		m_pMachine->ChangeState<TurtleShellLaunchState>();
		return;
	}

	if (turtleShell->IsAttack())
	{
		m_pMachine->ChangeState<TurtleShellSpinAttackStartState>();
		return;
	}

}

void TurtleShellNormalState::OnExit(TurtleShell * turtleShell)
{

}
