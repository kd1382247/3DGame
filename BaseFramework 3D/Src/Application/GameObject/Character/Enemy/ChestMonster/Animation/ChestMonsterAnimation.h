#pragma once

#include"../Animation/ChestMonsterAnimationType.h"
#include"../../../Animation/AnimationBase.h"

class ChestMonsterAnimation :public AnimationBase
{
public:

	ChestMonsterAnimation() {}
	~ChestMonsterAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& model);

	void Play(ChestMonsterAnimationType type);
	void RePlay(ChestMonsterAnimationType type);

private:

	void Animations(ChestMonsterAnimationType type);

	ChestMonsterAnimationType m_currentAnimation = ChestMonsterAnimationType::None;



};