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
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack1"), false,0.03f);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::Attack2:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack2"), false, 0.03f);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::Attack3:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack3"), false, 0.03f);
		m_animSpeed = 2.5f;
		break;
	case PlayerAnimationType::SpinAttack:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("SpinAttack"), false, 0.05f);
		m_animSpeed = 0.7f;
		break;
	case PlayerAnimationType::SpinChargeIDLE:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("SpinChargeIDLE"), true, 0.2f);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::SpinChargeBWD:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("SpinChargeBWD"), true, MoveBlendTime);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::SpinChargeFWD:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("SpinChargeFWD"), true, MoveBlendTime);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::SpinChargeLFT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("SpinChargeLFT"), true, MoveBlendTime);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::SpinChargeRGT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("SpinChargeRGT"), true, MoveBlendTime);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::SpinChargeStart:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("SpinChargeStart"),false,0.03);
		m_animSpeed = 5.0f;
		break;
	case PlayerAnimationType::GuardStart:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardStart"), false);
		m_animSpeed = 10.0f;
		break;
	case PlayerAnimationType::GuardEnd:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardEnd"),false, 0.03f);
		m_animSpeed = 5.0f;
		break;
	case PlayerAnimationType::GuardIDLE:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardIDLE"), true,MoveBlendTime);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::GuardWalkBWD:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardWalkBWD"), true, MoveBlendTime);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::GuardWalkFWD:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardWalkFWD"), true, MoveBlendTime);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::GuardWalkLFT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardWalkLFT"), true, MoveBlendTime);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::GuardWalkRGT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardWalkRGT"), true, MoveBlendTime);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::GuardHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardHit"), false, 0.05f);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::GuardBreak:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GuardBreak"), false, 0.05f);
		m_animSpeed = 1.6f;
		break;
	case PlayerAnimationType::Parry:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Parry"), false, 0.05f);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), false, 0.1f);
		m_animSpeed = 0.8f;
		break;
	case PlayerAnimationType::DieStay:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("DieStay"), true);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::Dizzy:

		break;
	case PlayerAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), false, 0.05f);
		m_animSpeed = 1.4f;
		break;
	case PlayerAnimationType::GetUp:
		break;
	case PlayerAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true,0.2f);
		m_animSpeed =1.0f;
		break;
	case PlayerAnimationType::IdleNormal:
		break;
	case PlayerAnimationType::JumpStart:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("JumpStart"), false,0.03);
		m_animSpeed = 2.5f;
		break;
	case PlayerAnimationType::JumpRise:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("JumpRise"), true,0.2);
		m_animSpeed = 0.8f;
		break;
	case PlayerAnimationType::JumpFall:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("JumpFall"), true,0.2);
		m_animSpeed = 0.8f;
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
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("MoveBWD"), true, MoveBlendTime);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::MoveFWD:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("MoveFWD"), true, MoveBlendTime);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::MoveLFT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("MoveLFT"), true, MoveBlendTime);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::MoveRGT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("MoveRGT"), true, MoveBlendTime);
		m_animSpeed = 1.0f;
		break;
	case PlayerAnimationType::SprintFWD:
		break;
	case PlayerAnimationType::Vectory:
		break;
	}

	m_currentAnimation = _animType;

}