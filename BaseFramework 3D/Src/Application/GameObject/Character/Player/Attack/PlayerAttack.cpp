#include "PlayerAttack.h"

#include"../Player.h"

#include"../../../../System/TimeManager/TimeManager.h"

void PlayerAttack::UpdateAttackInput()
{
	bool currentAttackButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

	m_isAttackTrigger = currentAttackButton&&!m_preAttackPressed;

	m_preAttackPressed = currentAttackButton;
	UpdateChargeAttackInput();
}

void PlayerAttack::UpdateAttackMove(Player& player)
{
	player.ApplyCameraRelativeMove(player.GetAttackMoveSpeed());

	player.FacingDirectionToCamera();
}

void PlayerAttack::SetAttackTiming(float &hitStart,float &hitEnd)
{	
	if (m_currentAttackCombo == AttackCombo::Attack1)
	{
		hitStart = 8;
		hitEnd = 13;
	}
	if (m_currentAttackCombo == AttackCombo::Attack2)
	{
		hitStart = 8;
		hitEnd = 13;
	}
	if (m_currentAttackCombo == AttackCombo::Attack3)
	{
		hitStart = 20;
		hitEnd = 25;
	}
}

void PlayerAttack::StartAttack()
{
	m_comboGraceActive = false;
	m_comboGraceTime = 0.0f;
	m_nextAttack = false;
	m_canCombo = false;
}

void PlayerAttack::StartComboGrace()
{
	m_comboGraceTime = 0.0f;
	m_comboGraceActive = true;
}

void PlayerAttack::UpdateComboReception(const float animFrameCount)
{

	// 3段目はこれ以上コンボを繋げない
	if (m_currentAttackCombo == AttackCombo::Attack3)
	{
		return;
	}

	if (HasNextCombo())
	{
		return;
	}

	// 受付開始タイミング
	if (animFrameCount >= m_comboInputStartFrame)
	{
		m_canCombo = true;
	}

	if (!m_canCombo)
	{
		return;
	}

	if (m_isAttackTrigger)
	{
		m_nextAttack = true;
		m_canCombo = false;
	}
}

void PlayerAttack::UpdateComboGrace()
{

	if (!m_comboGraceActive)
	{
		return;
	}

	float deltaTime = TimeManager::Instance().GetDeltaTime();

	m_comboGraceTime += deltaTime;

	if (m_isAttackTrigger)
	{
		UpdateComboState();
		m_comboGraceActive = false;
		return;
	}

	if (m_comboGraceTime >= m_comboGraceDuration)
	{
		m_comboGraceActive = false;
		ResetCombo();
	}
}

void PlayerAttack::ComboInputStartFrame()
{

	if (m_currentAttackCombo == AttackCombo::Attack1)
	{
		m_comboInputStartFrame = 18.0f;
	}
	if (m_currentAttackCombo == AttackCombo::Attack2)
	{
		m_comboInputStartFrame = 18.0f;
	}
}

void PlayerAttack::ResetCombo()
{
	m_currentAttackCombo = AttackCombo::Attack1;
	m_nextAttack = false;
	m_canCombo = false;
	m_comboGraceActive = false;
	m_comboGraceTime = 0.0f;
	m_comboInputStartFrame = 0.0f;
}

void PlayerAttack::UpdateComboState()
{
	switch (m_currentAttackCombo)
	{
	case AttackCombo::Attack1:
		m_currentAttackCombo = AttackCombo::Attack2;
		break;
	case AttackCombo::Attack2:
		m_currentAttackCombo = AttackCombo::Attack3;
		break;
	case AttackCombo::Attack3:
		ResetCombo();
		break;
	}
}

PlayerAnimationType PlayerAttack::GetAttackAnimation() const
{
	switch (m_currentAttackCombo)
	{
	case AttackCombo::Attack1:
		return PlayerAnimationType::Attack1;

	case AttackCombo::Attack2:
		return PlayerAnimationType::Attack2;

	case AttackCombo::Attack3:
		return PlayerAnimationType::Attack3;

	default:
		return PlayerAnimationType::Attack1;
	}
}

void PlayerAttack::UpdateChargeAttackInput()
{
	float deltaTime = TimeManager::Instance().GetDeltaTime();

	if (m_preAttackPressed)
	{
		m_attackHeldSeconds+=deltaTime;

		if (m_attackHeldSeconds >= m_longPressSeconds)
		{
			m_isAttackLongPressed = true;
		}
	}
	else
	{
		m_isAttackLongPressed = false;
		m_attackHeldSeconds = 0.0f;
	}

}

void PlayerAttack::StartCharge()
{
	m_chargeSeconds = 0.0f;
	m_isChargeComplete = false;
}

void PlayerAttack::UpdateChargeTime()
{

	float deltaTime = TimeManager::Instance().GetDeltaTime();
	m_chargeSeconds += deltaTime;

	if (m_chargeSeconds >= m_maxChargeSeconds)
	{
		m_chargeSeconds = m_maxChargeSeconds;
		m_isChargeComplete = true;
	}
}

void PlayerAttack::EndCharge()
{
	m_chargeSeconds = 0.0f;
	m_isChargeComplete = false;
}


PlayerAnimationType PlayerAttack::GetChargeMoveAnimation(const Player& player)const
{
	switch (player.GetMoveType())
	{
	case Player::MoveType::IDLE:
		return PlayerAnimationType::ChargeAttackIDLE;
	case Player::MoveType::BWD:
		return PlayerAnimationType::ChargeAttackBWD;
	case Player::MoveType::FWD:
		return PlayerAnimationType::ChargeAttackFWD;
	case Player::MoveType::LFT:
		return PlayerAnimationType::ChargeAttackLFT;
	case Player::MoveType::RGT:
		return PlayerAnimationType::ChargeAttackRGT;
	default:
		return PlayerAnimationType::ChargeAttackIDLE;
	}

}
