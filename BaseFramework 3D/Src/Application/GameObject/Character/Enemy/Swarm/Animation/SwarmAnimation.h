#pragma once

#include"../State/SwarmState.h"
#include"../../../Animation/AnimationBase.h"

class SwarmAnimation :public AnimationBase
{
public:

	SwarmAnimation() {}
	~SwarmAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& _model);

	void Play(SwarmAnimationType _animType);

private:

	SwarmAnimationType m_currentAnimation = SwarmAnimationType::None;



};