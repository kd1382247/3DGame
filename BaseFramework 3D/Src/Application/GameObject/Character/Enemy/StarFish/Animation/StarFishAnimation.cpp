#include "StarFishAnimation.h"

void StarFishAnimation::Init(std::shared_ptr<KdModelWork>& model)
{
	m_spModel = model;
	m_spAnimator = std::make_shared<KdAnimator>();

}

void StarFishAnimation::Play(StarFishAnimationType type)
{
	if (m_currentAnimation == type)
	{
		return;
	}

	Animations(type);
	m_currentAnimation = type;
}

void StarFishAnimation::RePlay(StarFishAnimationType type)
{
	// 同じアニメーションでも強制的に0から再生
	Animations(type);
	m_currentAnimation = type;
}

void StarFishAnimation::Animations(StarFishAnimationType type)
{
	switch (type)
	{
	case StarFishAnimationType::None:
		break;
	case StarFishAnimationType::Attack:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack"), false, 0.08f);
		m_animSpeed = 1.0f;
		break;
	case StarFishAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), false, 0.05f);
		m_animSpeed = 1.0f;
		break;
	case StarFishAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true, 0.2f);
		m_animSpeed = 1.5f;
		break;
	case StarFishAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true, 0.15f);
		m_animSpeed = 1.0f;
		break;

	case StarFishAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), false, 0.1f);
		m_animSpeed = 1.0f;
		break;
	}

}
