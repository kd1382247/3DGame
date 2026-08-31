#include "SlimeDamageState.h"

#include"../../Slime.h"

#include"SlimeNormalState.h"
#include"SlimeDieState.h"

void SlimeDamageState::Enter(Slime& slime)
{
	slime.PlayAnimation(SlimeAnimationType::GetHit);
}

void SlimeDamageState::Update(Slime & slime)
{
	if (slime.IsInOutro())
	{
		slime.ChangeState<SlimeDieState>();
		return;
	}

	if (slime.IsAnimationFinished())
	{
		slime.ChangeState<SlimeNormalState>();
		slime.SetDamaged(false);
		return;
	}
}

void SlimeDamageState::Exit(Slime & slime)
{
	slime.EndAttack();
}
