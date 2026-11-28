#include "MushroomDieState.h"

#include"../../Mushroom.h"

void MushroomDieState::OnStart(Mushroom* mushroom)
{
	mushroom->PlayAnimation(MushroomAnimationType::Die);
}

void MushroomDieState::OnUpdate(Mushroom * mushroom)
{

	// アニメーションが終わったかつ地面に着いていたら
	if (mushroom->IsAnimationFinished()&&mushroom->IsGrounded())
	{
		mushroom->CreateDeathSmoke();
		mushroom->Destroy();
	}
}

void MushroomDieState::OnExit(Mushroom * mushroom)
{

}
