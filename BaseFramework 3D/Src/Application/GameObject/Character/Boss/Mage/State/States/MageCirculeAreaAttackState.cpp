#include "MageCirculeAreaAttackState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

namespace
{
	// 詠唱開始から全方位魔法が発生するまでの時間(秒)
	constexpr float kCastDelay = 0.6f;
}

void MageCirculeAreaAttackState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack2);
	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageCirculeAreaAttackState::OnUpdate(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	if (!m_hasCast && m_castTimer >= kCastDelay)
	{
		mage->CirculeAreaAttack();
		m_hasCast = true;
	}

	if (mage->IsAnimationFinished())
	{
		m_pMachine->ChangeState<MageNormalState>();
	}
}

void MageCirculeAreaAttackState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
