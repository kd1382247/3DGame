#include "SlimeNormalState.h"

#include"../../Slime.h"

#include"SlimeAttackState.h"
#include"SlimeDamageState.h"
#include"SlimeLaunchState.h"
#include"SlimeDieState.h"

void SlimeNormalState::Enter(Slime& slime)
{
	slime.PlayAnimation(SlimeAnimationType::Idle);
}

void SlimeNormalState::Update(Slime & slime)
{

	slime.UpdateMove();

	if (slime.IsInOutro())
	{
		slime.ChangeState<SlimeDieState>();
		return;
	}

	if (slime.IsLaunch())
	{
		slime.ChangeState<SlimeLaunchState>();
		return;
	}

	if (slime.IsDamaged())
	{
		slime.ChangeState<SlimeDamageState>();
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
