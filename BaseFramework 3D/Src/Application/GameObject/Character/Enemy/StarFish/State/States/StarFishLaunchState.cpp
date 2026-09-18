#include "StarFishLaunchState.h"

#include"../../StarFish.h"

#include"StarFishNormalState.h"

void StarFishLaunchState::OnStart(StarFish* starFish)
{
	starFish->PlayAnimation(StarFishAnimationType::Idle);
}

void StarFishLaunchState::OnUpdate(StarFish * starFish)
{
	starFish->UpdateLaunch();

	if (!starFish->IsLaunch())
	{
		m_pMachine->ChangeState<StarFishNormalState>();
	}
}

void StarFishLaunchState::OnExit(StarFish * starFish)
{

}
