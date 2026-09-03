#include "MushroomAttackState.h"

#include"MushroomNormalState.h"

#include"../../Mushroom.h"

void MushroomAttackState::Enter(Mushroom& mushroom)
{
	mushroom.StartAttack();
	mushroom.PlayAnimation(MushroomAnimationType::Attack);
}

void MushroomAttackState::Update(Mushroom & mushroom)
{

	mushroom.UpdateAttackCollision();

	if (mushroom.IsAnimationFinished())
	{
		mushroom.ChangeState<MushroomNormalState>();
		return;
	}
}

void MushroomAttackState::Exit(Mushroom & mushroom)
{
	mushroom.EndAttack();
}
