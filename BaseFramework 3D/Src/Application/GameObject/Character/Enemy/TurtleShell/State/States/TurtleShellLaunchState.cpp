#include "TurtleShellLaunchState.h"

#include"../../TurtleShell.h"

#include"TurtleShellNormalState.h"

void TurtleShellLaunchState::OnStart(TurtleShell* turtleShell)
{
	turtleShell->PlayAnimation(TurtleShellAnimationType::Idle);
}

void TurtleShellLaunchState::OnUpdate(TurtleShell * turtleShell)
{
	turtleShell->UpdateLaunch();

	if (!turtleShell->IsLaunch())
	{
		m_pMachine->ChangeState<TurtleShellNormalState>();
	}
}

void TurtleShellLaunchState::OnExit(TurtleShell * turtleShell)
{

}
