#include "StarFishNormalState.h"

#include"../../StarFish.h"

#include"StarFishAttackState.h"
#include"StarFishLaunchState.h"

void StarFishNormalState::OnStart(StarFish* starFish)
{
	starFish->PlayAnimation(StarFishAnimationType::Idle);
}

void StarFishNormalState::OnUpdate(StarFish * starFish)
{

	starFish->UpdateMove();

	if (starFish->IsLaunch())
	{
		m_pMachine->ChangeState<StarFishLaunchState>();
		return;
	}

	if (starFish->IsAttack())
	{
		m_pMachine->ChangeState<StarFishAttackState>();
		return;
	}

}

void StarFishNormalState::OnExit(StarFish * starFish)
{

}
