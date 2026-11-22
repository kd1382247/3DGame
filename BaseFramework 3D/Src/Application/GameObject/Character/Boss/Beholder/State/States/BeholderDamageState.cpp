#include "BeholderDamageState.h"

#include"BeholderNormalState.h"

void BeholderDamageState::OnStart(Beholder* owner)
{
	owner->PlayAnimation(BeholderAnimationType::GetHit);
}

void BeholderDamageState::OnUpdate(Beholder * owner)
{

	if (owner->IsAnimationFinished())
	{
		m_pMachine->ChangeState<BeholderNormalState>();
	}
}

void BeholderDamageState::OnExit(Beholder * owner)
{

}
