#include "MushroomAttackState.h"

#include"MushroomNormalState.h"

#include"../../Mushroom.h"

void MushroomAttackState::OnStart(Mushroom* mushroom)
{
	mushroom->StartAttack();
	mushroom->PlayAnimation(MushroomAnimationType::Attack);
}

void MushroomAttackState::OnUpdate(Mushroom * mushroom)
{

	mushroom->UpdateAttackCollision();

	if (mushroom->IsAnimationFinished())
	{
		m_pMachine->ChangeState<MushroomNormalState>();
		return;
	}
}

void MushroomAttackState::OnExit(Mushroom * mushroom)
{
	mushroom->EndAttack();
}
