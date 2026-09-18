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

	starFish->UpdateAttackCollision();

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
