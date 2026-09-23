#include "SlimeDieState.h"

#include"../../Slime.h"

void SlimeDieState::OnStart(Slime* slime)
{
	slime->PlayAnimation(SlimeAnimationType::Die);
}

void SlimeDieState::OnUpdate(Slime * slime)
{
	if (slime->IsAnimationFinished())
	{
		slime->Split();
		slime->CreateDeathSmoke();
		slime->Destroy();
	}
}

void SlimeDieState::OnExit(Slime * slime)
{

}
