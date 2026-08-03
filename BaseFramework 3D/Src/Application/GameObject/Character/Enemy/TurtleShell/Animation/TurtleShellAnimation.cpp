#include "TurtleShellAnimation.h"

void TurtleShellAnimation::Init(std::shared_ptr<KdModelWork>& _model)
{
	m_spModel = _model;
	m_spAnimator = std::make_shared<KdAnimator>();

}

void TurtleShellAnimation::Play(TurtleShellAnimationType _animType)
{
	if (m_currentAnimation == _animType)
	{
		return;
	}

	switch (_animType)
	{
	case TurtleShellAnimationType::None:
		break;

	case TurtleShellAnimationType::RotationAttackST:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("RotationAttackST"), false);
		m_animSpeed = 1.0f;
		break;
	case TurtleShellAnimationType::RotationAttackRPT:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("RotationAttackRPT"), true);
		m_animSpeed = 1.0f;
		break;
	case TurtleShellAnimationType::GetHit:
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("GetHit"), true);
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
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Die"), true);
		m_animSpeed = 1.0f;
		break;
	}

	m_currentAnimation = _animType;
}
