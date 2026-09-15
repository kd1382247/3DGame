#include "PlayerGuard.h"

#include"../Player.h"

void PlayerGuard::UpdateGuardInput()
{
	bool currentGuardPressed = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

	m_guardTrigger = currentGuardPressed && !m_prevGuardPressed;

	m_prevGuardPressed = currentGuardPressed;

	// ガードの状態を変更
	if (m_guardTrigger)
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

void PlayerGuard::UpdateParryInput()
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
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
