#pragma once

#include"../State/StarFishState.h"
#include"../../../Animation/AnimationBase.h"

class StarFishAnimation :public AnimationBase
{
public:

	StarFishAnimation() {}
	~StarFishAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& _model);

	void Play(StarFishAnimationType _animType);

private:

	StarFishAnimationType m_currentAnimation = StarFishAnimationType::None;



};