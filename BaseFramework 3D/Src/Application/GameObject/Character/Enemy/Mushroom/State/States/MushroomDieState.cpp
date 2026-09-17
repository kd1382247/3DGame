#include "MushroomDieState.h"

#include"../../Mushroom.h"

void MushroomDieState::OnStart(Mushroom* mushroom)
{
	mushroom->PlayAnimation(MushroomAnimationType::Die);
}

void MushroomDieState::OnUpdate(Mushroom * mushroom)
{
	if (mushroom->IsAnimationFinished())
	{
		mushroom->Destroy();
	}
}

void MushroomDieState::OnExit(Mushroom * mushroom)
{

}
