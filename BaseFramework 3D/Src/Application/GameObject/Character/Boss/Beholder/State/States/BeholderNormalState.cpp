#include "BeholderNormalState.h"

#include"../../Beholder.h"

#include"BeholderSparkAttackState.h"
#include"BeholderChargeShotState.h"

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
		switch (beholder->SelectAttackPattern())
		{
		case Beholder::BeholderAttackPattern::SparkAttack:
			m_pMachine->ChangeState<BeholderSparkAttackState>();

			break;
		case Beholder::BeholderAttackPattern::ChargeShot:
			m_pMachine->ChangeState<BeholderChargeShotState>();

			break;
		}
	}
}

void BeholderNormalState::OnExit(Beholder* beholder)
{

}
