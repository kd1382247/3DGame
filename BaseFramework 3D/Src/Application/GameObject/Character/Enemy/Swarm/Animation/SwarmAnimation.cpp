#include "SwarmAnimation.h"

void SwarmAnimation::Init(std::shared_ptr<KdModelWork>& _model)
{
	m_spModel = _model;
	m_spAnimator = std::make_shared<KdAnimator>();

}

void SwarmAnimation::Play(SwarmAnimationType _animType)
{
	if (m_currentAnimation == _animType)
	{
		return;
	}

	switch (_animType)
	{
	case SwarmAnimationType::None:
		break;

	case SwarmAnimationType::Attack:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack"), false);
		m_animSpeed = 1.0f;
		break;
	case SwarmAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), true);
		m_animSpeed = 1.0f;
		break;
	case SwarmAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true);
		m_animSpeed = 1.0f;
		break;
	case SwarmAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true);
		m_animSpeed = 1.0f;
		break;

	case SwarmAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), true);
		m_animSpeed = 1.0f;
		break;
	}

	m_currentAnimation = _animType;
}
