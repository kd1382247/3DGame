#include "SlimeLaunchState.h"

#include"../../Slime.h"

#include"SlimeNormalState.h"

void SlimeLaunchState::OnStart(Slime* slime)
{
	slime->PlayAnimation(SlimeAnimationType::Idle);
}

void SlimeLaunchState::OnUpdate(Slime * slime)
{

	slime->UpdateLaunch();

	if (!slime->IsLaunch())
	{
		m_pMachine->ChangeState<SlimeNormalState>();
		return;
	}
}

void SlimeLaunchState::OnExit(Slime * slime)
{

}
