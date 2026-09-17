#include "TurtleShellSpinAttackStartState.h"

#include"../../TurtleShell.h"

#include"TurtleShellSpinAttackRepeatState.h"

void TurtleShellSpinAttackStartState::OnStart(TurtleShell* turtleShell)
{
	turtleShell->StartSpinAttack();
	turtleShell->PlayAnimation(TurtleShellAnimationType::SpinAttackST);
}

void TurtleShellSpinAttackStartState::OnUpdate(TurtleShell * turtleShell)
{
	if (turtleShell->IsAnimationFinished())
	{
		m_pMachine->ChangeState<TurtleShellSpinAttackRepeatState>();
	}
}

void TurtleShellSpinAttackStartState::OnExit(TurtleShell * turtleShell)
{

}
