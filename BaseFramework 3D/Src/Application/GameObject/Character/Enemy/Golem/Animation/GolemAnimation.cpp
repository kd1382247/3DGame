#include "GolemAnimation.h"

void GolemAnimation::Init(std::shared_ptr<KdModelWork>& _model)
{
	m_spModel = _model;
	m_spAnimator = std::make_shared<KdAnimator>();

}

void GolemAnimation::Play(GolemAnimationType _animType)
{
	if (m_currentAnimation == _animType)
	{
		return;
	}

	switch (_animType)
	{
	case GolemAnimationType::None:
		break;

	case GolemAnimationType::Attack1:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack1"), false);
		m_animSpeed = 1.0f;
		break;
	case GolemAnimationType::Attack2:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack2"), false);
		m_animSpeed = 1.0f;
		break;
	case GolemAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), true);
		m_animSpeed = 1.0f;
		break;
	case GolemAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true);
		m_animSpeed = 1.0f;
		break;
	case GolemAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true);
		m_animSpeed = 1.0f;
		break;

	case GolemAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), true);
		m_animSpeed = 1.0f;
		break;
	}

	m_currentAnimation = _animType;
}
