#pragma once

#include"../State/GolemState.h"
#include"../../../Animation/AnimationBase.h"

class GolemAnimation :public AnimationBase
{
public:

	GolemAnimation() {}
	~GolemAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& _model);

	void Play(GolemAnimationType _animType);

private:

	GolemAnimationType m_currentAnimation = GolemAnimationType::None;



};