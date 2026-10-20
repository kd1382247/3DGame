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
	mage->SetTargetDir();
	mage->UpdateFacingDirection();

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

	const auto& param = mage->GetParam().m_targetCircle;

	// 全弾撃ち終えている(最後の1発を撃った)かどうか
	const bool isLastShot = (m_shotCount >= param.m_shotCount);

	if (!isLastShot)
	{
		// 弾を出したあと、1発の開始から m_shotInterval 秒たったら、次の弾へ進む
		// (アニメより短ければ途中で切り替わり、長ければアニメが終わったあと待機で待つ)
		if (m_hasCast && m_castTimer >= param.m_shotInterval)
		{
			OnStart(mage);
			mage->RePlayAnimation(MageAnimationType::Attack1);
		}
		else if (mage->IsAnimationFinished())
		{
			mage->PlayAnimation(MageAnimationType::Idle);
		}
	}
	else if (mage->IsAnimationFinished())
	{
		// 最後の1発の円が消えるまで、待機アニメで待つ
		mage->PlayAnimation(MageAnimationType::Idle);
		m_isWaiting = true;
	}
}

void MageTargetCircleState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
