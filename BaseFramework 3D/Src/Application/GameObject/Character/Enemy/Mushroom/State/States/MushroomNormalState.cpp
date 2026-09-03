#include "MushroomNormalState.h"

#include"../../Mushroom.h"

#include"MushroomAttackState.h"
#include"MushroomLaunchState.h"

void MushroomNormalState::Enter(Mushroom& mushroom)
{
	mushroom.PlayAnimation(MushroomAnimationType::Idle);
}

void MushroomNormalState::Update(Mushroom & mushroom)
{

	mushroom.UpdateMove();

	if (mushroom.IsLaunch())
	{
		mushroom.ChangeState<MushroomLaunchState>();
		return;
	}

	if (mushroom.IsAttack())
	{
		mushroom.ChangeState<MushroomAttackState>();
		return;
	}

}

void MushroomNormalState::Exit(Mushroom & mushroom)
{

}
