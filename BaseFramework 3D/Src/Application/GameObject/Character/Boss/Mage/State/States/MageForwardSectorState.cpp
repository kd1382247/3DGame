#include "MageForwardSectorState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

namespace
{
	// 詠唱開始から前方範囲魔法が発生するまでの時間(秒)
	constexpr float kCastDelay = 0.35f;
}

void MageForwardSectorState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack1);
	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageForwardSectorState::OnUpdate(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	if (!m_hasCast && m_castTimer >= kCastDelay)
	{
		mage->CastForwardSector();
		m_hasCast = true;
	}

	if (mage->IsAnimationFinished())
	{
		m_pMachine->ChangeState<MageNormalState>();
	}
}

void MageForwardSectorState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
