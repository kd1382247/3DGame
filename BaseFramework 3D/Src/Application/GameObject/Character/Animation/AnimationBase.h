#pragma once

class AnimationBase
{
public:

	AnimationBase(){}
	~AnimationBase(){}

	void Init();
	void Update();
	bool IsFinished() const;


protected:

	std::shared_ptr<KdModelWork>m_spModel = nullptr;
	std::shared_ptr<KdAnimator> m_spAnimator = nullptr;

	float m_animSpeed = 0.5;

};