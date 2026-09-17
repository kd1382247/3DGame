#include "SlimeDamageState.h"

#include"../../Slime.h"

#include"SlimeNormalState.h"

void SlimeDamageState::OnStart(Slime* slime)
{
	slime->PlayAnimation(SlimeAnimationType::GetHit);
}

void SlimeDamageState::OnUpdate(Slime * slime)
{
	if (slime->IsAnimationFinished())
	{
		m_pMachine->ChangeState<SlimeNormalState>();
		return;
	}
}

void SlimeDamageState::OnExit(Slime * slime)
{
	slime->EndAttack();
}
