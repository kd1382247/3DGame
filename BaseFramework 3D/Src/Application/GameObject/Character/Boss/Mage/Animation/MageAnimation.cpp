#include "MageAnimation.h"

void MageAnimation::Init(std::shared_ptr<KdModelWork>& model)
{
	m_spModel = model;
	m_spAnimator = std::make_shared<KdAnimator>();

}

void MageAnimation::Play(MageAnimationType animType)
{
	if (m_currentAnimation == animType)
	{
		return;
	}

	Animations(animType);
	m_currentAnimation = animType;
}

void MageAnimation::RePlay(MageAnimationType animType)
{

	// 同じアニメーションでも強制的に0から再生
	Animations(animType);
	m_currentAnimation = animType;

}

void MageAnimation::Animations(MageAnimationType animType)
{
	switch (animType)
	{
	case MageAnimationType::None:
		break;
	case MageAnimationType::Attack1:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack1"), false);
		m_animSpeed = 1.4f;
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
}
