#include "BeholderAnimation.h"

void BeholderAnimation::Init(std::shared_ptr<KdModelWork>& _model)
{
	m_spModel = _model;
	m_spAnimator = std::make_shared<KdAnimator>();
}

void BeholderAnimation::Play(BeholderAnimationType _animType)
{
	if (m_currentAnimation == _animType)
	{
		return;
	}

	switch (_animType)
	{
	case BeholderAnimationType::None:
		break;
	case BeholderAnimationType::NormalAttack:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("NormalAttack"), false);
		m_animSpeed = 1.0f;
		break;
	case BeholderAnimationType::EnergyChargeST:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("EnergyChargeST"), false,0.1);
		m_animSpeed = 1.0f;
		break;
	case BeholderAnimationType::EnergyChargeRPT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("EnergyChargeRPT"), true,0.13);
		m_animSpeed = 1.0f;
		break;
	case BeholderAnimationType::EnergyShot:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("EnergyShot"), false,0.18f);
		m_animSpeed = 0.7f;
		break;
	case BeholderAnimationType::EnergyAttack:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("EnergyAttack"), false,0.14f);
		m_animSpeed = 1.0f;
		break;
	case BeholderAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), false,0.14f);
		m_animSpeed = 0.8f;
		break;
	case BeholderAnimationType::Dizzy:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Dizzy"), true,0.2f);
		m_animSpeed = 0.8f;
		break;
	case BeholderAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), false, 0.14f);
		m_animSpeed = 1.0f;
		break;
	case BeholderAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true,0.2f);
		m_animSpeed = 1.0f;
		break;
	case BeholderAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true,0.18f);
		m_animSpeed = 1.0f;
		break;
	case BeholderAnimationType::Vectory:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Vectory"), false);
		m_animSpeed = 1.0f;
		break;
	}
	
	m_currentAnimation = _animType;
}
