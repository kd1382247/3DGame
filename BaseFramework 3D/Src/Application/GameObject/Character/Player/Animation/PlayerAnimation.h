#pragma once

#include"../State/PlayerState.h"

#include"../../Animation/AnimationBase.h"

class PlayerAnimation:public AnimationBase
{
public:

	PlayerAnimation(){}
	~PlayerAnimation(){}

	void Init(const std::shared_ptr<KdModelWork>&_model);

	void Play(PlayerAnimationType _animType);
	

private:

	PlayerAnimationType m_currentAnimation = PlayerAnimationType::None;

};