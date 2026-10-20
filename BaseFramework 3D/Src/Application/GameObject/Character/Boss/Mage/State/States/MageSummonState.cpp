#include "MageSummonState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

void MageSummonState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::EnemySpawnerChant);
	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageSummonState::OnUpdate(Mage* mage)
{
	if (!m_hasCast)
	{
		m_castTimer += mage->GetDeltaTime();

		if (m_castTimer >= mage->GetParam().m_summon.m_castDelay)
		{
			// スポナーに、敵の生成を頼む
			mage->RequestSummon();
			m_hasCast = true;
		}

		return;
	}

	// 詠唱のアニメが終わったら通常状態に戻る(敵はスポナーが出し続ける)
	if (mage->IsAnimationFinished())
	{
		m_pMachine->ChangeState<MageNormalState>();
	}
}

void MageSummonState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
