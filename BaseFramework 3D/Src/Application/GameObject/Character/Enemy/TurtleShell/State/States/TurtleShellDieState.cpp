#include "TurtleShellDieState.h"

#include"../../TurtleShell.h"

void TurtleShellDieState::OnStart(TurtleShell* turtleShell)
{
	turtleShell->PlayAnimation(TurtleShellAnimationType::Die);
}

void TurtleShellDieState::OnUpdate(TurtleShell * turtleShell)
{

	// アニメーションが終わったかつ地面に着いていたら
	if (turtleShell->IsAnimationFinished()&&turtleShell->IsGrounded())
	{
		turtleShell->CreateDeathSmoke();
		turtleShell->Destroy();
	}
}

void TurtleShellDieState::OnExit(TurtleShell * turtleShell)
{

}
