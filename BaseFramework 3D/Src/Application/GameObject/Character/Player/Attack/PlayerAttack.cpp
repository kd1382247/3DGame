#include "PlayerAttack.h"

#include"../Player.h"

#include"../../../Camera/CameraBase.h"

#include"../../../../System/TimeManager/TimeManager.h"

void PlayerAttack::UpdateAttackInput()
{
	bool currentAttackButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

	m_isAttackPressed = currentAttackButton;

}

void PlayerAttack::UpdateAttackMove(Player& player)
{
	

	Math::Matrix camRotYMat = Math::Matrix::Identity;

	auto spCamera = player.GetCamera().lock();
	if (!spCamera)
	{
		return;
	}

	camRotYMat = spCamera->GetRotationYMatrix();
	player.SetMoveDir(Math::Vector3::TransformNormal(player.GetMoveDir(), camRotYMat));

	player.FacingDirectionToCamera();

	Math::Vector3 dir = player.GetMoveDir();
	
	if (dir.LengthSquared() > 0.0f)
	{
		dir.Normalize();
	}

	float moveSpeed= player.GetAttackMoveSpeed();

	float deltaTime = TimeManager::Instance().GetDeltaTime();

	Math::Vector3 move = dir * (moveSpeed * 60.0f) * deltaTime;
	player.AddPendingMove(move);
}

void PlayerAttack::SetAttackTiming(float &hitStart,float &hitEnd)const
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

	if (m_isAttackPressed)
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

	if (m_isAttackPressed)
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
