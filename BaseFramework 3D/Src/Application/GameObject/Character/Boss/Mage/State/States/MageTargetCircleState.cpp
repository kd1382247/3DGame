#include "MageTargetCircleState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

namespace
{
	// 詠唱開始から魔法円が発生するまでの時間(秒)
	constexpr float kCastDelay = 0.4f;
}

void MageTargetCircleState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack1);
	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageTargetCircleState::OnUpdate(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	if (!m_hasCast && m_castTimer >= kCastDelay)
	{
		mage->CastTargetCircle();
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

void MageTargetCircleState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
