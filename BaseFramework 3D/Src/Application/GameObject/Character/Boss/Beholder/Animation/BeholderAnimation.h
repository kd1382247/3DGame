#pragma once

#include"BeholderAnimationType.h"
#include"../../../Animation/AnimationBase.h"

class BeholderAnimation :public AnimationBase
{
public:

	BeholderAnimation() {}
	~BeholderAnimation(){}

	void Init(std::shared_ptr<KdModelWork>&_model);

	void Play(BeholderAnimationType _animType);

	BeholderAnimationType GetPlayAnimType()const {return m_currentAnimation; }

private:

	BeholderAnimationType m_currentAnimation = BeholderAnimationType::None;



};