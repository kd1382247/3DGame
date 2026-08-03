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
	case BeholderAnimationType::BeamAttackST:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("BeamAttackST"), false);
		m_animSpeed = 1.0f;
		break;
	case BeholderAnimationType::BeamAttackRPT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("BeamAttackRPT"), true);
		m_animSpeed = 1.0f;
		break;
	case BeholderAnimationType::BeamAttackEND:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("BeamAttackEND"), false);
		m_animSpeed = 1.0f;
		break;
	case BeholderAnimationType::EnergyAttack:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("EnergyAttack"), false);
		m_animSpeed = 1.0f;
		break;
	case BeholderAnimationType::Die:
		break;
	case BeholderAnimationType::Dizzy:
		break;
	case BeholderAnimationType::GetHit:
		break;
	case BeholderAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true);
		m_animSpeed = 1.0f;
		break;
	case BeholderAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true);
		m_animSpeed = 1.0f;
		break;
	case BeholderAnimationType::Vectory:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Vectory"), false);
		m_animSpeed = 1.0f;
		break;
	}
	
	m_currentAnimation = _animType;
}
