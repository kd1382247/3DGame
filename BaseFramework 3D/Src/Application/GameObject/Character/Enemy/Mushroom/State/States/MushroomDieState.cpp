#include "MushroomDieState.h"

#include"../../Mushroom.h"

void MushroomDieState::Enter(Mushroom& mushroom)
{
	mushroom.PlayAnimation(MushroomAnimationType::Die);
}

void MushroomDieState::Update(Mushroom & mushroom)
{
	if (mushroom.IsAnimationFinished())
	{
		mushroom.Destroy();
	}
}

void MushroomDieState::Exit(Mushroom & mushroom)
{

}
