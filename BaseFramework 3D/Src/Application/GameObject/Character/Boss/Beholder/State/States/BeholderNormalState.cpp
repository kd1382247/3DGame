#include "BeholderNormalState.h"

#include"../../Beholder.h"

#include"BeholderNormalAttackState.h"

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
		m_pMachine->ChangeState<BeholderNormalAttackState>();
	}


}

void BeholderNormalState::OnExit(Beholder* beholder)
{

}
