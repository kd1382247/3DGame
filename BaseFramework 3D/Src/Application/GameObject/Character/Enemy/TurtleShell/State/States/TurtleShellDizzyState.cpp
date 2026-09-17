#include "TurtleShellDizzyState.h"

#include"../../TurtleShell.h"

#include"TurtleShellNormalState.h"

void TurtleShellDizzyState::OnStart(TurtleShell* turtleShell)
{
	turtleShell->StartDizzy();
	turtleShell->PlayAnimation(TurtleShellAnimationType::Dizzy);
}

void TurtleShellDizzyState::OnUpdate(TurtleShell * turtleShell)
{

	if (turtleShell->DizyyRemaining())
	{
		m_pMachine->ChangeState<TurtleShellNormalState>();
		return;
	}
}

void TurtleShellDizzyState::OnExit(TurtleShell * turtleShell)
{
	turtleShell->EndDizzy();
}
