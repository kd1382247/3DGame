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

private:

	BombAnimationType m_currentAnimation = BombAnimationType::None;



};