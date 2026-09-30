#include "MageTargetCircleState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

void MageTargetCircleState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack1);
	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageTargetCircleState::OnUpdate(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	if (!m_hasCast && m_castTimer >= mage->GetParam().m_targetCircleCastDelay)
	{
		mage->CastTargetCircle();
		m_hasCast = true;
		m_shotCount++;
	}

	if (mage->IsAnimationFinished())
	{

		if (m_shotCount < mage->GetParam().m_targetCircleShotCount)
		{
			OnStart(mage);
			mage->RePlayAnimation(MageAnimationType::Attack1);
		}
		else
		{
			m_pMachine->ChangeState<MageNormalState>();
		}

	}
}

void MageTargetCircleState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
