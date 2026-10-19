#include "MageNovaCircleState.h"

#include"../../Mage.h"
#include"MageNormalState.h"
#include"../../MageMagicCircle/MageMagicCircle.h"

void MageNovaCircleState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack2);
	m_castTimer = 0.0f;
	m_hasCast = false;
	m_isWaiting = false;
	m_wpMagicCircle.reset();
}

void MageNovaCircleState::OnUpdate(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	if (!m_hasCast && m_castTimer >= mage->GetParam().m_novaCircle.m_castDelay)
	{
		m_wpMagicCircle = mage->CastNovaCircle();
		m_hasCast = true;
	}

	// 攻撃のアニメが終わったら、待機アニメに切り替えて円が消えるのを待つ
	if (!m_isWaiting)
	{
		if (m_hasCast && mage->IsAnimationFinished())
		{
			mage->PlayAnimation(MageAnimationType::Idle);
			m_isWaiting = true;
		}

		return;
	}

	// 魔法円が無くなった(=攻撃終了した)らNormalStateへ戻る
	auto spMagicCircle = m_wpMagicCircle.lock();

	if (!spMagicCircle || spMagicCircle->IsFinished())
	{
		m_pMachine->ChangeState<MageNormalState>();
	}
}

void MageNovaCircleState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
