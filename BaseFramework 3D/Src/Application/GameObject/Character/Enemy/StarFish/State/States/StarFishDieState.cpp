#include "StarFishDieState.h"

#include"../../StarFish.h"

void StarFishDieState::OnStart(StarFish* starFish)
{
	starFish->PlayAnimation(StarFishAnimationType::Die);
}

void StarFishDieState::OnUpdate(StarFish * starFish)
{
	if (starFish->IsAnimationFinished())
	{
		starFish->Destroy();
	}
}

void StarFishDieState::OnExit(StarFish * starFish)
{

}
