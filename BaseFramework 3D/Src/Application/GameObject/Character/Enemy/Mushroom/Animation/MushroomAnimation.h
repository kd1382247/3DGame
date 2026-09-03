#pragma once

#include"MushroomAnimationType.h"
#include"../../../Animation/AnimationBase.h"

class MushroomAnimation :public AnimationBase
{
public:

	MushroomAnimation() {}
	~MushroomAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& model);

	void Play(MushroomAnimationType type);
	void RePlay(MushroomAnimationType type);

private:


	void Animations(MushroomAnimationType type);

	MushroomAnimationType m_currentAnimation = MushroomAnimationType::None;



};