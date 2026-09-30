#include "MageBoltState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

void MageBoltState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack1);
	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageBoltState::OnUpdate(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	mage->SetTargetDir();
	mage->UpdateFacingDirection();

	if (!m_hasCast && m_castTimer >= mage->GetParam().m_boltCastDelay)
	{
		mage->FireBolt();
		m_hasCast = true;
		m_shotCount++;
	}

	if (mage->IsAnimationFinished())
	{
		if (m_shotCount < mage->GetParam().m_boltShotCount)
		{
			OnStart(mage);
			// アニメーションを再再生
			mage->RePlayAnimation(MageAnimationType::Attack1);
		}
		else
		{
			m_pMachine->ChangeState<MageNormalState>();
		}
	}
}

void MageBoltState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
