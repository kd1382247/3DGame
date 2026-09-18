#pragma once

class StarFish;

#include"../../../../StateMachine/StateBase.h"

class StarFishNormalState :public StateBase<StarFish>
{
public:

	void OnStart(StarFish* starFish)override;
	void OnUpdate(StarFish* starFish)override;
	void OnExit(StarFish* starFish)override;

};
