#pragma once

#include"../State/MushroomState.h"
#include"../../../Animation/AnimationBase.h"

class MushroomAnimation :public AnimationBase
{
public:

	MushroomAnimation() {}
	~MushroomAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& _model);

	void Play(MushroomAnimationType _animType);

private:

	MushroomAnimationType m_currentAnimation = MushroomAnimationType::None;



};