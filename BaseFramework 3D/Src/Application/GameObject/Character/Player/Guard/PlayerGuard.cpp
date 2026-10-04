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
		return PlayerAnimationType::GuardIDLE;

	case GuardState::GuardHit:
		return PlayerAnimationType::GuardHit;

	case GuardState::GuardBreak:
		return PlayerAnimationType::GuardBreak;

	default:
		return PlayerAnimationType::GuardIDLE;
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

	if(m_guardState!=GuardState::GuardHit)
	{
		m_guardState = GuardState::GuardHit;
	}

	// もしガード中に受ける上限値を超えたら
	// ガードを強制終了する
	if (m_guardHitCount >= m_pParam->m_guardBreakCount)
	{
		m_guardHitCount = 0;
		m_guardState = GuardState::GuardBreak;
	}
}

PlayerAnimationType PlayerGuard::GetGuardMoveAnimation(const PlayerInput::MoveType moveType) const
{
	switch (moveType)
	{
	case PlayerInput::MoveType::IDLE:
		return PlayerAnimationType::GuardIDLE;
	case PlayerInput::MoveType::BWD:
		return PlayerAnimationType::GuardWalkBWD;
	case PlayerInput::MoveType::FWD:
		return PlayerAnimationType::GuardWalkFWD;
	case PlayerInput::MoveType::LFT:
		return PlayerAnimationType::GuardWalkLFT;
	case PlayerInput::MoveType::RGT:
		return PlayerAnimationType::GuardWalkRGT;
	default:
		return PlayerAnimationType::GuardIDLE;
	}
}

bool PlayerGuard::IsInGuardRange(const Math::Vector3& forward, const Math::Vector3& knockBackDir) const
{
	
	Math::Vector3 forwardDir = forward;
	forwardDir.y = 0.0f;
	

	Math::Vector3 targetDir = knockBackDir;
	targetDir.y = 0.0f;

	// 長さが0の時はtrue(ガード成功)を返す
	if (targetDir.LengthSquared() <= 0.000001f ||
		forwardDir.LengthSquared() <= 0.000001f)
	{
		return true;
	}

	// 正規化
	forwardDir.Normalize();
	targetDir.Normalize();

	Math::Vector3 toAttackDir = -targetDir;

	float dot = forwardDir.Dot(toAttackDir);

	// 角度を半分にして、ラジアン単位に変換
	float halfAngleRad = DirectX::XMConvertToRadians(m_pParam->m_guardAngle / 2);

	float minDot = std::cos(halfAngleRad);


	return dot >= minDot;
}
