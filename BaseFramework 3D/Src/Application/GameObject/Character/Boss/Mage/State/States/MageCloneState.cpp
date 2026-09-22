#include "MageCloneState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

namespace
{
	// 詠唱開始から分身が発生するまでの時間(秒)
	constexpr float kCastDelay = 0.6f;
}

void MageCloneState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack2);
	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageCloneState::OnUpdate(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	if (!m_hasCast && m_castTimer >= kCastDelay)
	{
		mage->SpawnClones();
		m_hasCast = true;
	}

	if (mage->IsAnimationFinished())
	{
		m_pMachine->ChangeState<MageNormalState>();
	}
}

void MageCloneState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
