#include "AnimationBase.h"

void AnimationBase::Init()
{}

void AnimationBase::Update()
{
	if (!m_spModel || !m_spAnimator)
	{
		return;
	}

	// アニメーション更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes(), m_animSpeed);

	if (m_spModel->NeedCalcNodeMatrices())
	{
		m_spModel->CalcNodeMatrices();
	}
}

bool AnimationBase::IsFinished() const
{
	if (!m_spAnimator)
	{
		return false;
	}
	return m_spAnimator->IsAnimationEnd();
}
