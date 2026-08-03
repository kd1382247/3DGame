#include "MageAnimation.h"

void MageAnimation::Init(std::shared_ptr<KdModelWork>& _model)
{
	m_spModel = _model;
	m_spAnimator = std::make_shared<KdAnimator>();

}

void MageAnimation::Play(MageAnimationType _animType)
{
	if (m_currentAnimation == _animType)
	{
		return;
	}

	switch (_animType)
	{
	case MageAnimationType::None:
		break;
	case MageAnimationType::Attack1:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack1"), false);
		m_animSpeed = 1.0f;
		break;
	case MageAnimationType::Attack2:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack2"), false);
		m_animSpeed = 1.0f;
		break;
	case MageAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), true);
		m_animSpeed = 1.0f;
		break;
	case MageAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true);
		m_animSpeed = 1.0f;
		break;
	case MageAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true);
		m_animSpeed = 1.0f;
		break;

	case MageAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), true);
		m_animSpeed = 1.0f;
		break;
	}

	m_currentAnimation = _animType;
}
