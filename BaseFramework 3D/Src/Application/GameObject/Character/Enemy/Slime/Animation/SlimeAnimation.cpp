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
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack"), false, 0.08f);
		m_animSpeed = 1.0f;
		break;
	case SlimeAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), false, 0.05f);
		m_animSpeed = 1.0f;
		break;
	case SlimeAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true, 0.2f);
		m_animSpeed = 1.0f;
		break;
	case SlimeAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true, 0.15f);
		m_animSpeed = 1.0f;
		break;

	case SlimeAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), false, 0.1f);
		m_animSpeed = 0.7f;
		break;
	}
}
