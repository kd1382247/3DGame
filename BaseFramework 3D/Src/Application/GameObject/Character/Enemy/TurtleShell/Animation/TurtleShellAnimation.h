#pragma once

#include"../State/TurtleShellState.h"
#include"../../../Animation/AnimationBase.h"

class TurtleShellAnimation :public AnimationBase
{
public:

	TurtleShellAnimation() {}
	~TurtleShellAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& _model);

	void Play(TurtleShellAnimationType _animType);

private:

	TurtleShellAnimationType m_currentAnimation = TurtleShellAnimationType::None;



};