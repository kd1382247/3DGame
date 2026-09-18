#include "BombDamageState.h"

#include"../../Bomb.h"

#include"BombNormalState.h"

void BombDamageState::OnStart(Bomb* bomb)
{
	bomb->PlayAnimation(BombAnimationType::GetHit);
}

void BombDamageState::OnUpdate(Bomb * bomb)
{

	if (bomb->IsAnimationFinished())
	{
		m_pMachine->ChangeState<BombNormalState>();
	}
}

void BombDamageState::OnExit(Bomb * bomb)
{

}
