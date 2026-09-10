#include "AnimationBase.h"

#include"../../../System/TimeManager/TimeManager.h"

void AnimationBase::Init()
{}

void AnimationBase::Update(const float deltaTime)
{
	if (!m_spModel || !m_spAnimator)
	{
		return;
	}

	float animDelta = deltaTime *60.0f* m_animSpeed;

	// アニメーション更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes(),animDelta);

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
