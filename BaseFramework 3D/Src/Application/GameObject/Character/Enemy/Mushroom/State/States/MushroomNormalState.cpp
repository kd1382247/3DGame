#include "MushroomNormalState.h"

#include"../../Mushroom.h"

#include"MushroomAttackState.h"
#include"MushroomLaunchState.h"

void MushroomNormalState::OnStart(Mushroom* mushroom)
{
	mushroom->PlayAnimation(MushroomAnimationType::Idle);
}

void MushroomNormalState::OnUpdate(Mushroom * mushroom)
{

	mushroom->UpdateMove();

	if (mushroom->IsLaunch())
	{
		m_pMachine->ChangeState<MushroomLaunchState>();
		return;
	}

	if (mushroom->IsAttack())
	{
		m_pMachine->ChangeState<MushroomAttackState>();
		return;
	}

}

void MushroomNormalState::OnExit(Mushroom * mushroom)
{

}
