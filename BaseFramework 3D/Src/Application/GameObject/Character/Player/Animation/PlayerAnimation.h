#pragma once

#include"../State/PlayerState.h"

class PlayerAnimation
{
public:

	PlayerAnimation(){}
	~PlayerAnimation(){}

	void Init(const std::shared_ptr<KdModelWork>&_model);

	void Change(PlayerAnimationType _nextAnim);
	void Update();

	bool IsFinished() const;
	

private:

	std::shared_ptr<KdModelWork>m_spModel = nullptr;
	std::shared_ptr<KdAnimator> m_spAnimator = nullptr;

	PlayerAnimationType m_currentAnimation = PlayerAnimationType::None;

	float m_animSpeed = 0.5;

};