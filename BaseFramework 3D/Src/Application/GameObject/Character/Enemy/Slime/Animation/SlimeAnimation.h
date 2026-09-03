#pragma once

#include"../Animation/SlimeAnimationType.h"
#include"../../../Animation/AnimationBase.h"

class SlimeAnimation :public AnimationBase
{
public:

	SlimeAnimation() {}
	~SlimeAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& model);

	void Play(SlimeAnimationType type);
	void RePlay(SlimeAnimationType type);

private:

	void Animations(SlimeAnimationType type);

	SlimeAnimationType m_currentAnimation = SlimeAnimationType::None;



};