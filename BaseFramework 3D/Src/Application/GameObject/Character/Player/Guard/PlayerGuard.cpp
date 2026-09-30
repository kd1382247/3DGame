#include "PlayerGuard.h"

void PlayerGuard::Update(const PlayerInput& input)
{
	// ガードの状態を変更
	if (input.IsGuardTrigger())
	{
		// ガード解除
		if (m_guardState == GuardState::Guard)
		{
			m_guardState = GuardState::GuardCancel;
		}
		else
		{
			m_guardState = GuardState::Guard;
		}
	}
}

void PlayerGuard::UpdateParry(const PlayerInput& input)
{
	if (input.IsAttackDown())
	{
		m_guardState = GuardState::Parry;
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

	case GuardState::Parry:
		return PlayerAnimationType::Parry;

	default:
		return PlayerAnimationType::Defend;
	}
}
