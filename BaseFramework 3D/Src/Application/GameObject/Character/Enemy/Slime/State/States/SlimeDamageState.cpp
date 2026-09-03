#include "SlimeDamageState.h"

#include"../../Slime.h"

#include"SlimeNormalState.h"

void SlimeDamageState::Enter(Slime& slime)
{
	slime.PlayAnimation(SlimeAnimationType::GetHit);
}

void SlimeDamageState::Update(Slime & slime)
{
	if (slime.IsAnimationFinished())
	{
		slime.ChangeState<SlimeNormalState>();
		return;
	}
}

void SlimeDamageState::Exit(Slime & slime)
{
	slime.EndAttack();
}
