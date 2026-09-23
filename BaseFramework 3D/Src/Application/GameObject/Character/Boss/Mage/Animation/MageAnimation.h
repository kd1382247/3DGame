#pragma once

#include"../State/MageState.h"
#include"../../../Animation/AnimationBase.h"

class MageAnimation :public AnimationBase
{
public:

	MageAnimation() {}
	~MageAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& model);

	void Play(MageAnimationType animType);
	void RePlay(MageAnimationType animType);

private:

	void Animations(MageAnimationType animType);

	MageAnimationType m_currentAnimation = MageAnimationType::None;



};