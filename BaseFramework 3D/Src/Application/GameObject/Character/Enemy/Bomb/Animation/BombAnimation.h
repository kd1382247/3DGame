#pragma once

#include"../State/BombState.h"
#include"../../../Animation/AnimationBase.h"

class BombAnimation :public AnimationBase
{
public:

	BombAnimation() {}
	~BombAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& _model);

	void Play(BombAnimationType _animType);
	void RePlay(BombAnimationType type);

private:

	void Animations(BombAnimationType type);

	BombAnimationType m_currentAnimation = BombAnimationType::None;

};