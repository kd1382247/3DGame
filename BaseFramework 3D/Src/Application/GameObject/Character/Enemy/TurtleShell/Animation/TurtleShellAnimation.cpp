#include "TurtleShellAnimation.h"

void TurtleShellAnimation::Init(std::shared_ptr<KdModelWork>& model)
{
	m_spModel = model;
	m_spAnimator = std::make_shared<KdAnimator>();

}

void TurtleShellAnimation::Play(TurtleShellAnimationType type)
{
	if (m_currentAnimation == type)
	{
		return;
	}

	Animations(type);
	m_currentAnimation = type;
}

void TurtleShellAnimation::RePlay(TurtleShellAnimationType type)
{

	Animations(type);
	m_currentAnimation = type;
}

void TurtleShellAnimation::Animations(TurtleShellAnimationType type)
{
	switch (type)
	{
	case TurtleShellAnimationType::None:
		break;

	case TurtleShellAnimationType::SpinAttackST:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("RotationAttackST"), false);
		m_animSpeed = 0.5f;
		break;
	case TurtleShellAnimationType::SpinAttackRPT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("RotationAttackRPT"), true);
		m_animSpeed = 3.0f;
		break;
	case TurtleShellAnimationType::Dizzy:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Dizzy"), true);
		m_animSpeed = 1.4f;
		break;
	case TurtleShellAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), false);
		m_animSpeed = 1.0f;
		break;
	case TurtleShellAnimationType::Idle:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Idle"), true);
		m_animSpeed = 1.0f;
		break;
	case TurtleShellAnimationType::Walk:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Walk"), true);
		m_animSpeed = 1.0f;
		break;

	case TurtleShellAnimationType::Die:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), false);
		m_animSpeed = 1.0f;
		break;
	}

}
