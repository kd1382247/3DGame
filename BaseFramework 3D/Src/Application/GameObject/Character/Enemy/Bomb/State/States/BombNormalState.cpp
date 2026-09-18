#include "BombNormalState.h"

#include"../../Bomb.h"

#include"BombChargeState.h"
#include"BombLaunchState.h"

void BombNormalState::OnStart(Bomb* bomb)
{
	bomb->PlayAnimation(BombAnimationType::Idle);
}

void BombNormalState::OnUpdate(Bomb * bomb)
{

	bomb->UpdateMove();

	if (bomb->IsLaunch())
	{
		m_pMachine->ChangeState<BombLaunchState>();
		return;
	}

	if (bomb->IsAttack())
	{
		m_pMachine->ChangeState<BombChargeState>();
		return;
	}

}

void BombNormalState::OnExit(Bomb * bomb)
{

}
