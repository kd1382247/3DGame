#include "SlimeLaunchState.h"

#include"../../Slime.h"

#include"SlimeNormalState.h"

void SlimeLaunchState::Enter(Slime& slime)
{
	slime.PlayAnimation(SlimeAnimationType::Idle);
}

void SlimeLaunchState::Update(Slime & slime)
{
	
	slime.UpdateLaunch();

	if (!slime.IsLaunch())
	{
		slime.ChangeState<SlimeNormalState>();
		return;
	}
}

void SlimeLaunchState::Exit(Slime & slime)
{

}
