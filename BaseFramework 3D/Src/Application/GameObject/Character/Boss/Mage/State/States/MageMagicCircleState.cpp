#include "MageMagicCircleState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

namespace
{
	// 詠唱開始から魔法円が発生するまでの時間(秒)
	constexpr float kCastDelay = 0.4f;
}

void MageMagicCircleState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack1);
	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageMagicCircleState::OnUpdate(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	if (!m_hasCast && m_castTimer >= kCastDelay)
	{
		mage->CastMagicCircle();
		m_hasCast = true;
		m_shotCount++;
	}

	if (mage->IsAnimationFinished())
	{

		if (m_shotCount < maxShot)
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

void MageMagicCircleState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
