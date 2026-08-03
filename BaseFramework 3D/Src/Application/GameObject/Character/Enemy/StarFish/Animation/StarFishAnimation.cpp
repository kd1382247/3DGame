#include "StarFishAnimation.h"

void StarFishAnimation::Init(std::shared_ptr<KdModelWork>& _model)
{
	m_spModel = _model;
	m_spAnimator = std::make_shared<KdAnimator>();

}

void StarFishAnimation::Play(StarFishAnimationType _animType)
{
	if (m_currentAnimation == _animType)
	{
		return;
	}

	switch (_animType)
	{
	case StarFishAnimationType::None:
		break;

	case StarFishAnimationType::Attack:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack"), false);
		m_animSpeed = 1.0f;
		break;
	case StarFishAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), true);
		m_animSpeed = 1.0f;
		break;
	case StarFishAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true);
		m_animSpeed = 1.0f;
		break;
	case StarFishAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true);
		m_animSpeed = 1.0f;
		break;

	case StarFishAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), true);
		m_animSpeed = 1.0f;
		break;
	}

	m_currentAnimation = _animType;
}
