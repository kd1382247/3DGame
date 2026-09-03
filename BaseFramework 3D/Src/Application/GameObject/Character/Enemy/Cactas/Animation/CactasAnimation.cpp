#include "CactasAnimation.h"

void CactasAnimation::Init(std::shared_ptr<KdModelWork>& model)
{
	m_spModel = model;
	m_spAnimator = std::make_shared<KdAnimator>();

}

void CactasAnimation::Play(CactasAnimationType type)
{
	if (m_currentAnimation == type)
	{
		return;
	}

	Animations(type);
	m_currentAnimation = type;
}

void CactasAnimation::RePlay(CactasAnimationType type)
{
	// 同じアニメーションでも強制的に0から再生
	Animations(type);
	m_currentAnimation = type;
}

void CactasAnimation::Animations(CactasAnimationType type)
{
	switch (type)
	{
	case CactasAnimationType::None:
		break;
	case CactasAnimationType::Attack:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack"), false);
		m_animSpeed = 1.0f;
		break;
	case CactasAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), false);
		m_animSpeed = 1.0f;
		break;
	case CactasAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true);
		m_animSpeed = 1.5f;
		break;
	case CactasAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true);
		m_animSpeed = 1.0f;
		break;

	case CactasAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), false);
		m_animSpeed = 1.0f;
		break;
	}

}
