#include "MushroomAnimation.h"

void MushroomAnimation::Init(std::shared_ptr<KdModelWork>& _model)
{
	m_spModel = _model;
	m_spAnimator = std::make_shared<KdAnimator>();

}

void MushroomAnimation::Play(MushroomAnimationType type)
{
	if (m_currentAnimation == type)
	{
		return;
	}

	Animations(type);
	m_currentAnimation = type;
}

void MushroomAnimation::RePlay(MushroomAnimationType type)
{

	Animations(type);
	m_currentAnimation = type;
}

void MushroomAnimation::Animations(MushroomAnimationType type)
{
	switch (type)
	{
	case MushroomAnimationType::None:
		break;

	case MushroomAnimationType::Attack:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack"), false);
		m_animSpeed = 1.0f;
		break;
	case MushroomAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), false);
		m_animSpeed = 1.0f;
		break;
	case MushroomAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true);
		m_animSpeed = 1.0f;
		break;
	case MushroomAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true);
		m_animSpeed = 1.0f;
		break;

	case MushroomAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), false);
		m_animSpeed = 1.0f;
		break;
	}

}
