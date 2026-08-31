#include "SlimeDieState.h"

#include"../../Slime.h"

void SlimeDieState::Enter(Slime& slime)
{
	slime.PlayAnimation(SlimeAnimationType::Die);
}

void SlimeDieState::Update(Slime & slime)
{
	if (slime.IsAnimationFinished())
	{
		slime.Split();
		slime.Destroy();
	}
}

void SlimeDieState::Exit(Slime & slime)
{

}
