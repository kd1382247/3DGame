#include "PlayerAnimation.h"

void PlayerAnimation::Init(const std::shared_ptr<KdModelWork>& _model)
{
	m_spModel = _model;
	m_spAnimator = std::make_shared<KdAnimator>();
	
	Play(PlayerAnimationType::Idle);
}

void PlayerAnimation::Play(PlayerAnimationType _animType)
{
	if (m_currentAnimation == _animType)
	{
		return;
	}

	switch (_animType)
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
		m_animSpeed = 2.5f;
		break;
	case PlayerAnimationType::AttackSpin:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("AttackSpin"), false);
		m_animSpeed = 0.7f;
		break;
	case PlayerAnimationType::ChargeAttackIDLE:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("ChargeAttackIDLE"), true);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::ChargeAttackBWD:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("ChargeAttackBWD"), true);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::ChargeAttackFWD:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("ChargeAttackFWD"), true);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::ChargeAttackLFT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("ChargeAttackLFT"), true);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::ChargeAttackRGT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("ChargeAttackRGT"), true);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::GuardIDLE:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardIDLE"), true);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::GuardWalkBWD:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardWalkBWD"), true);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::GuardWalkFWD:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardWalkFWD"), true);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::GuardWalkLFT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardWalkLFT"), true);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::GuardWalkRGT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardWalkRGT"), true);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::GuardHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardHit"), false);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::GuardBreak:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardBreak"), false);
		m_animSpeed = 1.6f;
		break;
	case PlayerAnimationType::Parry:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Parry"), false);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::Die:

		break;
	case PlayerAnimationType::DieStay:

		break;
	case PlayerAnimationType::Dizzy:

		break;
	case PlayerAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), false);
		m_animSpeed = 1.4f;
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

	m_currentAnimation = _animType;

}