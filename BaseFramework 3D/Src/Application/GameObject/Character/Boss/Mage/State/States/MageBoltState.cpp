#include "MageBoltState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

namespace
{
	// 詠唱開始から魔法弾を発射するまでの時間(秒)
	constexpr float kCastDelay = 0.3f;
}

void MageBoltState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack1);
	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageBoltState::OnUpdate(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	if (!m_hasCast && m_castTimer >= kCastDelay)
	{
		mage->FireBolt();
		m_hasCast = true;
	}

	if (mage->IsAnimationFinished())
	{
		m_pMachine->ChangeState<MageNormalState>();
	}
}

void MageBoltState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
