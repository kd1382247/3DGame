#pragma once

#include"../Animation/CactasAnimationType.h"
#include"../../../Animation/AnimationBase.h"

class CactasAnimation :public AnimationBase
{
public:

	CactasAnimation() {}
	~CactasAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& model);

	void Play(CactasAnimationType type);
	void RePlay(CactasAnimationType type);

private:

	void Animations(CactasAnimationType type);

	CactasAnimationType m_currentAnimation = CactasAnimationType::None;

};