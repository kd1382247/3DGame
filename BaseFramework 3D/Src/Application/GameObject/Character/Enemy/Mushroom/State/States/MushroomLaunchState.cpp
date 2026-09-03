#include "MushroomLaunchState.h"

#include"../../Mushroom.h"

#include"MushroomNormalState.h"

void MushroomLaunchState::Enter(Mushroom& mushroom)
{
	mushroom.PlayAnimation(MushroomAnimationType::Idle);
}

void MushroomLaunchState::Update(Mushroom & mushroom)
{
	mushroom.UpdateLaunch();

	if (!mushroom.IsLaunch())
	{
		mushroom.ChangeState<MushroomNormalState>();
	}
}

void MushroomLaunchState::Exit(Mushroom & mushroom)
{

}
