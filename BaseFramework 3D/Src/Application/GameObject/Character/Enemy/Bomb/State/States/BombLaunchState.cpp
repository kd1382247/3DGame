#include "BombLaunchState.h"

#include"../../Bomb.h"

#include"BombNormalState.h"

void BombLaunchState::OnStart(Bomb* bomb)
{
	bomb->PlayAnimation(BombAnimationType::Idle);
}

void BombLaunchState::OnUpdate(Bomb * bomb)
{
	bomb->UpdateLaunch();

	if (!bomb->IsLaunch())
	{
		m_pMachine->ChangeState<BombNormalState>();
	}
}

void BombLaunchState::OnExit(Bomb * bomb)
{

}
