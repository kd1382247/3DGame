#include "BombAnimation.h"

void BombAnimation::Init(std::shared_ptr<KdModelWork>& model)
{
	m_spModel = model;
	m_spAnimator = std::make_shared<KdAnimator>();

}

void BombAnimation::Play(BombAnimationType type)
{
	if (m_currentAnimation == type)
	{
		return;
	}

	Animations(type);
	m_currentAnimation = type;
}

void BombAnimation::RePlay(BombAnimationType type)
{
	// 同じアニメーションでも強制的に0から再生
	Animations(type);
	m_currentAnimation = type;
}

void BombAnimation::Animations(BombAnimationType type)
{
	switch (type)
	{
	case BombAnimationType::None:
		break;
	case BombAnimationType::Attack:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack"), false, 0.03f);
		m_animSpeed = 1.0f;
		break;
	case BombAnimationType::AttackCharge:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("AttackCharge"), true, 0.08f);
		m_animSpeed = 1.0f;
		break;
	case BombAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), false, 0.05f);
		m_animSpeed = 1.0f;
		break;
	case BombAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true, 0.2f);
		m_animSpeed = 1.5f;
		break;
	case BombAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true, 0.15f);
		m_animSpeed = 1.0f;
		break;

	case BombAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), false, 0.1f);
		m_animSpeed = 1.0f;
		break;
	}

}
