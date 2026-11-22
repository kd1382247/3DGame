#include "BeholderNormalState.h"

#include"../../Beholder.h"

#include"BeholderSparkAttackState.h"
#include"BeholderChargeShotState.h"
#include"BeholderDizzyState.h"



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
		//m_pMachine->ChangeState<BeholderSparkAttackState>();
		//m_pMachine->ChangeState<BeholderDizzyState>();
		m_pMachine->ChangeState<BeholderChargeShotState>();
	}


}

void BeholderNormalState::OnExit(Beholder* beholder)
{

}
