#include "BeholderDizzyState.h"

#include"BeholderNormalState.h"

void BeholderDizzyState::OnStart(Beholder* owner)
{
	// 目の追従を止める
	owner->SetEyeTracking(false);

	owner->StartDizzy();

	owner->PlayAnimation(BeholderAnimationType::Dizzy);
}

void BeholderDizzyState::OnUpdate(Beholder * owner)
{
	owner->UpdateDizzy();

	if (owner->IsFloatRecovered())
	{
		m_pMachine->ChangeState<BeholderNormalState>();
	}
}

void BeholderDizzyState::OnExit(Beholder * owner)
{

	// 目の追従を始める
	owner->SetEyeTracking(true);

	owner->EndAttack();
}
