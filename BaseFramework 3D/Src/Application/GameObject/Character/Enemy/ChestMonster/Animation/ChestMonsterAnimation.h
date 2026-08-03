#pragma once

#include"../State/ChestMonsterState.h"
#include"../../../Animation/AnimationBase.h"

class ChestMonsterAnimation :public AnimationBase
{
public:

	ChestMonsterAnimation() {}
	~ChestMonsterAnimation() {}

	void Init(std::shared_ptr<KdModelWork>& _model);

	void Play(ChestMonsterAnimationType _animType);

private:

	ChestMonsterAnimationType m_currentAnimation = ChestMonsterAnimationType::None;



};