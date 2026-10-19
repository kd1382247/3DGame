#include "MageTargetCircleState.h"

#include"../../Mage.h"
#include"MageNormalState.h"
#include"../../MageMagicCircle/MageMagicCircle.h"

void MageTargetCircleState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack1);
	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageTargetCircleState::OnUpdate(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	if (!m_hasCast && m_castTimer >= mage->GetParam().m_targetCircle.m_castDelay)
	{
		m_wpMagicCircle = mage->CastTargetCircle();
		m_hasCast = true;
		m_shotCount++;
	}

	// 全弾撃ち終えたあとは、最後の円が消えるのを待つ
	if (m_isWaiting)
	{
		auto spMagicCircle = m_wpMagicCircle.lock();

		if (!spMagicCircle || spMagicCircle->IsFinished())
		{
			m_pMachine->ChangeState<MageNormalState>();
		}

		return;
	}

	if (mage->IsAnimationFinished())
	{

		if (m_shotCount < mage->GetParam().m_targetCircle.m_shotCount)
		{
			OnStart(mage);
			mage->RePlayAnimation(MageAnimationType::Attack1);
		}
		else
		{
			// 最後の1発の円が消えるまで、待機アニメで待つ
			mage->PlayAnimation(MageAnimationType::Idle);
			m_isWaiting = true;
		}

	}
}

void MageTargetCircleState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
