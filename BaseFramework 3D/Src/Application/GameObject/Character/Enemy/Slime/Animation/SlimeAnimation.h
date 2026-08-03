#pragma once

#include"../State/SlimeState.h"
#include"../../../Animation/AnimationBase.h"

class SlimeAnimation :public AnimationBase
{
public:

	SlimeAnimation() {}
	~SlimeAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& _model);

	void Play(SlimeAnimationType _animType);

private:

	SlimeAnimationType m_currentAnimation = SlimeAnimationType::None;



};