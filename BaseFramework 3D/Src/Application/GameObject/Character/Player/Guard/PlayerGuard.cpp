#include "PlayerGuard.h"

void PlayerGuard::UpdateTimer(const float deltaTime)
{

	m_guardHitResetTimer += deltaTime;

	// 一度ガード中に攻撃を受けてから
	// 一定時間で攻撃を受ける上限値とタイマーをリセット
	if (m_guardHitResetTimer >= m_pParam->m_guardHitResetTime)
	{
		m_guardHitResetTimer = 0.0f;
		m_guardHitCount = 0;
	}
	
}

PlayerAnimationType PlayerGuard::GetGuardAnimation() const
{
	switch (m_guardState)
	{
	case GuardState::Guard:
		return PlayerAnimationType::Defend;

	case GuardState::GuardHit:
		return PlayerAnimationType::DefendHit;

	default:
		return PlayerAnimationType::Defend;
	}
}

void PlayerGuard::NotifyGuardHit()
{

	if (m_guardState == GuardState::GuardBreak)
	{
		return;
	}

	// ガード中に受けた攻撃をカウント
	m_guardHitCount++;

	// 攻撃を受けるたびタイマーをリセット
	m_guardHitResetTimer = 0.0f;

	m_guardState = GuardState::GuardHit;

	// もしガード中に受ける上限値を超えたら
	// ガードを強制終了する
	if (m_guardHitCount >= m_pParam->m_guardBreakCount)
	{
		m_guardHitCount = 0;
		m_guardState = GuardState::GuardBreak;
	}
}
