#include "MushroomLaunchState.h"

#include"../../Mushroom.h"

#include"MushroomNormalState.h"

void MushroomLaunchState::OnStart(Mushroom* mushroom)
{
	mushroom->PlayAnimation(MushroomAnimationType::Idle);
}

void MushroomLaunchState::OnUpdate(Mushroom * mushroom)
{
	mushroom->UpdateLaunch();

	if (!mushroom->IsLaunch())
	{
		m_pMachine->ChangeState<MushroomNormalState>();
	}
}

void MushroomLaunchState::OnExit(Mushroom * mushroom)
{

}
