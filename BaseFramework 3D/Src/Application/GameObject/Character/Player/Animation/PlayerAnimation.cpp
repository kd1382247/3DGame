#include "PlayerAnimation.h"

void PlayerAnimation::Init(const std::shared_ptr<KdModelWork>& _model)
{

	m_spModel = _model;

	m_spAnimator = std::make_shared<KdAnimator>();
	
	Change(PlayerAnimationType::Idle);
}

void PlayerAnimation::Change(PlayerAnimationType _nextAnim)
{
	if (m_currentAnimation == _nextAnim)
	{
		return;
	}

	switch (_nextAnim)
	{
	case PlayerAnimationType::None:
		break;
	case PlayerAnimationType::Attack1:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack1"), false);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::Attack2:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack2"), false);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::Attack3:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack3"), false);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::AttackSpin:
		break;
	case PlayerAnimationType::Defend:
		break;
	case PlayerAnimationType::DefendHit:
		break;
	case PlayerAnimationType::Die:
		break;
	case PlayerAnimationType::DieStay:
		break;
	case PlayerAnimationType::Dizzy:
		break;
	case PlayerAnimationType::GetHit:
		break;
	case PlayerAnimationType::GetUp:
		break;
	case PlayerAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true);
		m_animSpeed =1.0f;
		break;
	case PlayerAnimationType::IdleNormal:
		break;
	case PlayerAnimationType::JumpStart:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("JumpStart"), false);
		m_animSpeed = 2.5f;
		break;
	case PlayerAnimationType::JumpAir:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("JumpLoop"), true);
		m_animSpeed = 0.5f;
		break;
	case PlayerAnimationType::JumpLand:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("JumpLand"), false);
		m_animSpeed = 2.5f; 
		break;
	case PlayerAnimationType::JumpSpin:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("JumpSpin"), false);
		break;
	case PlayerAnimationType::LevelUp:
		break;
	case PlayerAnimationType::MoveBWD:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("MoveBWD"), true);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::MoveFWD:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("MoveFWD"), true);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::MoveLFT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("MoveLFT"), true);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::MoveRGT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("MoveRGT"), true);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::SprintFWD:
		break;
	case PlayerAnimationType::Vectory:
		break;
	}

	m_currentAnimation = _nextAnim;

}

void PlayerAnimation::Update()
{
	if (!m_spModel || !m_spAnimator)
	{
		return;
	}

	// アニメーション更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes(), m_animSpeed);

	if (m_spModel->NeedCalcNodeMatrices())
	{
		m_spModel->CalcNodeMatrices();
	}

}

bool PlayerAnimation::IsFinished() const
{

	if (!m_spAnimator)
	{
		return false;
	}
	return m_spAnimator->IsAnimationEnd();
}
