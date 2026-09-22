#include "StarFishAttackState.h"

#include"StarFishNormalState.h"

#include"../../StarFish.h"

void StarFishAttackState::OnStart(StarFish* starFish)
{
	starFish->StartAttack();
	starFish->PlayAnimation(StarFishAnimationType::Attack);
}

void StarFishAttackState::OnUpdate(StarFish * starFish)
{
	if (!starFish->HasFiredBullet())
	{
		starFish->UpdateBulletFireTiming();
	}

	if (starFish->IsAnimationFinished())
	{
		m_pMachine->ChangeState<StarFishNormalState>();
		return;
	}
}

void StarFishAttackState::OnExit(StarFish * starFish)
{
	starFish->EndAttack();
}
