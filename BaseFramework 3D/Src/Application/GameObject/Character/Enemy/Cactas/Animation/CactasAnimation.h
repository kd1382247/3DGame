#pragma once

#include"../Animation/CactasAnimationType.h"
#include"../../../Animation/AnimationBase.h"

class CactasAnimation :public AnimationBase
{
public:

	CactasAnimation() {}
	~CactasAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& _model);

	void Play(CactasAnimationType _animType);

private:

	CactasAnimationType m_currentAnimation = CactasAnimationType::None;



};