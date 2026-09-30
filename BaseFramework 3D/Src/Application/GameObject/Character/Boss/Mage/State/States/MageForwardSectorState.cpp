#include "MageForwardSectorState.h"

#include"../../Mage.h"
#include"MageNormalState.h"
#include"../../../../../MageMagicSector/MageMagicSector.h"

namespace
{
	// 詠唱開始から前方範囲魔法が発生するまでの時間(秒)
	constexpr float kCastDelay = 0.35f;
}

void MageForwardSectorState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack2);
	m_castTimer = 0.0f;
	m_hasCast = false;
}

void MageForwardSectorState::OnUpdate(Mage* mage)
{
	m_castTimer += mage->GetDeltaTime();

	if (!m_hasCast && m_castTimer >= kCastDelay)
	{
		m_wpMageMagicSector=mage->CastForwardSector();
		m_hasCast = true;
	}

	if (m_hasCast)
	{
		// Beam本体が無くなった(=攻撃終了した)らNormalStateへ戻る
		auto spMageMagicSector = m_wpMageMagicSector.lock();

		if (!spMageMagicSector || spMageMagicSector->IsFinished())
		{
			m_pMachine->ChangeState<MageNormalState>();
		}
	}

}

void MageForwardSectorState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
