#include "ChestMonsterAnimation.h"

void ChestMonsterAnimation::Init(std::shared_ptr<KdModelWork>& model)
{
	m_spModel = model;
	m_spAnimator = std::make_shared<KdAnimator>();

}

void ChestMonsterAnimation::Play(ChestMonsterAnimationType type)
{
	if (m_currentAnimation == type)
	{
		return;
	}

	Animations(type);
	m_currentAnimation = type;
}

void ChestMonsterAnimation::RePlay(ChestMonsterAnimationType type)
{
	// 同じアニメーションでも再生
	Animations(type);
	m_currentAnimation = type;
}

void ChestMonsterAnimation::Animations(ChestMonsterAnimationType type)
{

	switch (type)
	{
	case ChestMonsterAnimationType::None:
		break;

	case ChestMonsterAnimationType::Attack:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Attack"), false);
		m_animSpeed = 1.0f;
		break;
	case ChestMonsterAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), false);
		m_animSpeed = 1.0f;
		break;

	case ChestMonsterAnimationType::GenerateMinion:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GenerateMinion"), false);
		m_animSpeed = 0.8f;
		break;
	case ChestMonsterAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true);
		m_animSpeed = 1.0f;
		break;
	case ChestMonsterAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true);
		m_animSpeed = 1.0f;
		break;

	case ChestMonsterAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), false);
		m_animSpeed = 1.0f;
		break;
	}

}
