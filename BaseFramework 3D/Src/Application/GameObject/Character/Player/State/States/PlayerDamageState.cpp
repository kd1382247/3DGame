#include "PlayerDamageState.h"

#include"../../Player.h"

#include"PlayerNormalState.h"
#include"PlayerAttackState.h"

void PlayerDamageState::OnStart(Player* owner)
{
	owner->SetStateType(PlayerStateType::DamageState);
	owner->PlayAnimation(PlayerAnimationType::GetHit);
}

void PlayerDamageState::OnUpdate(Player * owner)
{

	if (owner->IsAttackPressed())
	{
		m_pMachine->ChangeState<PlayerAttackState>();
		return;
	}

	owner->UpdateMove();

	if (owner->IsAnimationFinished())
	{
		m_pMachine->ChangeState<PlayerNormalState>();
		return;
	}
}

void PlayerDamageState::OnExit(Player * owner)
{

}
