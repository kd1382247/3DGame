#include "MageNovaCircleState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

void MageNovaCircleState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack2);
	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageNovaCircleState::OnUpdate(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	if (!m_hasCast && m_castTimer >= mage->GetParam().m_novaCircleCastDelay)
	{
		mage->CastNovaCircle();
		m_hasCast = true;
	}

	if (mage->IsAnimationFinished())
	{
		m_pMachine->ChangeState<MageNormalState>();
	}
}

void MageNovaCircleState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
