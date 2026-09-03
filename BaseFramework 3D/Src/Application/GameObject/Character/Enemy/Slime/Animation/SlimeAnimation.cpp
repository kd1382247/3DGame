#include "SlimeAnimation.h"

void SlimeAnimation::Init(std::shared_ptr<KdModelWork>& model)
{
	m_spModel = model;
	m_spAnimator = std::make_shared<KdAnimator>();

}

void SlimeAnimation::Play(SlimeAnimationType type)
{
	if (m_currentAnimation == type)
	{
		return;
	}

	Animations(type);

	m_currentAnimation = type;
}

void SlimeAnimation::RePlay(SlimeAnimationType type)
{
	Animations(type);

	m_currentAnimation = type;
}

void SlimeAnimation::Animations(SlimeAnimationType type)
{
	switch (type)
	{
	case SlimeAnimationType::None:
		break;

	case SlimeAnimationType::Attack:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack"), false);
		m_animSpeed = 1.0f;
		break;
	case SlimeAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), false);
		m_animSpeed = 1.0f;
		break;
	case SlimeAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true);
		m_animSpeed = 1.0f;
		break;
	case SlimeAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true);
		m_animSpeed = 1.0f;
		break;

	case SlimeAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), false);
		m_animSpeed = 1.0f;
		break;
	}
}
