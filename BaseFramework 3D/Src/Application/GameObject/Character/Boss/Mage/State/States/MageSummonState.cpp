#include "MageSummonState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

namespace
{
	// 詠唱開始から煙演出が始まるまでの時間(秒)
	constexpr float kCastDelay = 0.4f;

	// 煙演出が始まってから敵が出現するまでの時間(秒)
	constexpr float kSummonDelay = 0.6f;
}

void MageSummonState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Attack1);
	m_castTimer = 0.0f;
	m_hasCast = false;
	m_summonTimer = 0.0f;
	m_hasSpawned = false;
}

void MageSummonState::OnUpdate(Mage* mage)
{
	if (!m_hasCast)
	{
		m_castTimer += mage->GetDeltaTime();

		if (m_castTimer >= kCastDelay)
		{
			// 出現座標を決めて、煙エフェクトを再生する
			mage->PrepareSummonPositions();
			m_hasCast = true;
		}

		return;
	}

	if (!m_hasSpawned)
	{
		m_summonTimer += mage->GetDeltaTime();

		if (m_summonTimer >= kSummonDelay)
		{
			// 用意しておいた座標に敵を生成する
			mage->SpawnEnemies();
			m_hasSpawned = true;
		}

		return;
	}

	// 敵の出現が完了したら通常状態に戻る
	m_pMachine->ChangeState<MageNormalState>();
}

void MageSummonState::OnExit(Mage* mage)
{
	mage->EndAttack();
}
