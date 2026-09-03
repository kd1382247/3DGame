#include "SlimeNormalState.h"

#include"../../Slime.h"

#include"SlimeAttackState.h"
#include"SlimeLaunchState.h"

void SlimeNormalState::Enter(Slime& slime)
{
	slime.PlayAnimation(SlimeAnimationType::Idle);
}

void SlimeNormalState::Update(Slime & slime)
{

	slime.UpdateMove();

	if (slime.IsLaunch())
	{
		slime.ChangeState<SlimeLaunchState>();
		return;
	}

	if (slime.IsAttack())
	{
		slime.ChangeState<SlimeAttackState>();
		return;
	}
	
}

void SlimeNormalState::Exit(Slime & slime)
{

}
