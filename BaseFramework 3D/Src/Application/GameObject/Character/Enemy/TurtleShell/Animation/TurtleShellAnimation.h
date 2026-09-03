#pragma once

#include"TurtleShellAnimationType.h"
#include"../../../Animation/AnimationBase.h"

class TurtleShellAnimation :public AnimationBase
{
public:

	TurtleShellAnimation() {}
	~TurtleShellAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& model);

	void Play(TurtleShellAnimationType type);
	void RePlay(TurtleShellAnimationType type);

private:

	void Animations(TurtleShellAnimationType type);

	TurtleShellAnimationType m_currentAnimation = TurtleShellAnimationType::None;

};