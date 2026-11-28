#include "StarFishDieState.h"

#include"../../StarFish.h"

void StarFishDieState::OnStart(StarFish* starFish)
{
	starFish->PlayAnimation(StarFishAnimationType::Die);
}

void StarFishDieState::OnUpdate(StarFish * starFish)
{

	// アニメーションが終わったかつ地面に着いていたら
	if (starFish->IsAnimationFinished()&&starFish->IsGrounded())
	{
		starFish->CreateDeathSmoke();
		starFish->Destroy();
	}
}

void StarFishDieState::OnExit(StarFish * starFish)
{

}
