#pragma once

#include"../State/MageState.h"
#include"../../../Animation/AnimationBase.h"

class MageAnimation :public AnimationBase
{
public:

	MageAnimation() {}
	~MageAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& _model);

	void Play(MageAnimationType _animType);

private:

	MageAnimationType m_currentAnimation = MageAnimationType::None;



};