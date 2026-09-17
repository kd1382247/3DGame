#include "SlimeNormalState.h"

#include"../../Slime.h"

#include"SlimeAttackState.h"
#include"SlimeLaunchState.h"

void SlimeNormalState::OnStart(Slime* slime)
{
	slime->PlayAnimation(SlimeAnimationType::Idle);
}

void SlimeNormalState::OnUpdate(Slime * slime)
{

	slime->UpdateMove();

	if (slime->IsLaunch())
	{
		m_pMachine->ChangeState<SlimeLaunchState>();
		return;
	}

	if (slime->IsAttack())
	{
		m_pMachine->ChangeState<SlimeAttackState>();
		return;
	}

}

void SlimeNormalState::OnExit(Slime * slime)
{

}
