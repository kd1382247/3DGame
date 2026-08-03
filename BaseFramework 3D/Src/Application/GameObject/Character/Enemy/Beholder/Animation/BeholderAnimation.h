#pragma once

#include"../State/BeholderState.h"
#include"../../../Animation/AnimationBase.h"

class BeholderAnimation :public AnimationBase
{
public:

	BeholderAnimation() {}
	~BeholderAnimation(){}

	void Init(std::shared_ptr<KdModelWork>&_model);

	void Play(BeholderAnimationType _animType);

private:

	BeholderAnimationType m_currentAnimation = BeholderAnimationType::None;



};