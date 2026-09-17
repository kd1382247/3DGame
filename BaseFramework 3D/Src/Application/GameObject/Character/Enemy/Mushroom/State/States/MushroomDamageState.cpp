#include "MushroomDamageState.h"

#include"../../Mushroom.h"

#include"MushroomNormalState.h"

void MushroomDamageState::OnStart(Mushroom* mushroom)
{
	mushroom->PlayAnimation(MushroomAnimationType::GetHit);
}

void MushroomDamageState::OnUpdate(Mushroom * mushroom)
{

	if (mushroom->IsAnimationFinished())
	{
		m_pMachine->ChangeState<MushroomNormalState>();
	}
}

void MushroomDamageState::OnExit(Mushroom * mushroom)
{
	mushroom->EndAttack();
}
