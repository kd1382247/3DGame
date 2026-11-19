#include "BeholderNormalState.h"

#include"../../Beholder.h"

#include"BeholderSparkAttackState.h"

void BeholderNormalState::OnStart(Beholder* beholder)
{
	beholder->PlayAnimation(BeholderAnimationType::Idle);
}

void BeholderNormalState::OnUpdate(Beholder* beholder)
{
	beholder->UpdateFacingDirection();
	beholder->UpdateMove();

	if (beholder->IsAttack())
	{
		m_pMachine->ChangeState<BeholderSparkAttackState>();
	}


}

void BeholderNormalState::OnExit(Beholder* beholder)
{

}
