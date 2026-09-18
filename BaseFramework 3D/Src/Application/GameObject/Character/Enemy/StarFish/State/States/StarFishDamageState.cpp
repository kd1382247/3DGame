#include "StarFishDamageState.h"

#include"../../StarFish.h"

#include"StarFishNormalState.h"

void StarFishDamageState::OnStart(StarFish* starFish)
{
	starFish->PlayAnimation(StarFishAnimationType::GetHit);
}

void StarFishDamageState::OnUpdate(StarFish * starFish)
{

	if (starFish->IsAnimationFinished())
	{
		m_pMachine->ChangeState<StarFishNormalState>();
	}
}

void StarFishDamageState::OnExit(StarFish * starFish)
{
	starFish->EndAttack();
}
