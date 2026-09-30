#include "PlayerAttack.h"

namespace
{
	// コンボ段ごとのデータ表(AttackComboの並び順と同じにする)
	// 段を増やす時は、AttackComboに追加してここに1行足す
	//   { アニメーション, { hitStart, hitEnd, trailStart, trailEnd }, 次のコンボ入力を受け付け始めるフレーム }
	const PlayerAttack::AttackData kAttackTable[] =
	{
		// Attack1
		{ PlayerAnimationType::Attack1, {  8.0f, 13.0f, 0.0f, 18.0f }, 18.0f },
		// Attack2
		{ PlayerAnimationType::Attack2, {  8.0f, 13.0f, 0.0f, 18.0f }, 18.0f },
		// Attack3(最後の段なので、次のコンボ入力は受け付けない)
		{ PlayerAnimationType::Attack3, { 20.0f, 25.0f, 0.0f, 30.0f },  0.0f },
	};
}

const PlayerAttack::AttackData& PlayerAttack::GetCurrentAttackData() const
{
	return kAttackTable[static_cast<int>(m_currentAttackCombo)];
}

void PlayerAttack::StartAttack()
{
	m_comboGraceActive = false;
	m_comboGraceTime = 0.0f;
	m_nextAttack = false;
	m_canCombo = false;

	m_comboInputStartFrame = GetCurrentAttackData().comboInputStartFrame;
}

void PlayerAttack::StartComboGrace()
{
	m_comboGraceTime = 0.0f;
	m_comboGraceActive = true;
}

void PlayerAttack::UpdateComboReception(const PlayerInput& input, const float animFrameCount)
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

	if (input.IsAttackTrigger())
	{
		m_nextAttack = true;
		m_canCombo = false;
	}
}

void PlayerAttack::UpdateComboGrace(const PlayerInput& input, const float deltaTime)
{

	if (!m_comboGraceActive)
	{
		return;
	}

	m_comboGraceTime += deltaTime;

	if (input.IsAttackTrigger())
	{
		NextCombo();
		m_comboGraceActive = false;
		return;
	}

	if (m_comboGraceTime >= m_comboGraceDuration)
	{
		m_comboGraceActive = false;
		ResetCombo();
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

void PlayerAttack::NextCombo()
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

void PlayerAttack::StartCharge()
{
	m_chargeSeconds = 0.0f;
	m_isChargeComplete = false;
}

void PlayerAttack::UpdateChargeTime(const float deltaTime)
{
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


PlayerAnimationType PlayerAttack::GetChargeMoveAnimation(const PlayerInput::MoveType moveType) const
{
	switch (moveType)
	{
	case PlayerInput::MoveType::IDLE:
		return PlayerAnimationType::ChargeAttackIDLE;
	case PlayerInput::MoveType::BWD:
		return PlayerAnimationType::ChargeAttackBWD;
	case PlayerInput::MoveType::FWD:
		return PlayerAnimationType::ChargeAttackFWD;
	case PlayerInput::MoveType::LFT:
		return PlayerAnimationType::ChargeAttackLFT;
	case PlayerInput::MoveType::RGT:
		return PlayerAnimationType::ChargeAttackRGT;
	default:
		return PlayerAnimationType::ChargeAttackIDLE;
	}

}
