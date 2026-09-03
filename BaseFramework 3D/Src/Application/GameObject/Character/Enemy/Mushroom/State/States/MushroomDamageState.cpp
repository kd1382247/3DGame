#include "MushroomDamageState.h"

#include"../../Mushroom.h"

#include"MushroomNormalState.h"

void MushroomDamageState::Enter(Mushroom& mushroom)
{
	mushroom.PlayAnimation(MushroomAnimationType::GetHit);
}

void MushroomDamageState::Update(Mushroom & mushroom)
{

	if (mushroom.IsAnimationFinished())
	{
		mushroom.ChangeState<MushroomNormalState>();
	}
}

void MushroomDamageState::Exit(Mushroom & mushroom)
{
	mushroom.EndAttack();
}
