#include "PlayerNormalState.h"


#include"../../Player.h"

#include"PlayerAttackState.h"
#include"PlayerChargeAttackState.h"
#include"PlayerJumpStartState.h"
#include"PlayerGuardState.h"

void PlayerNormalState::OnStart(Player* owner)
{
	owner->SetStateType(PlayerStateType::NormalState);
}

void PlayerNormalState::OnUpdate(Player* owner)
{
	owner->UpdateMove();

	// 移動アニメーション
	if (owner->IsMovePressed())
	{
		owner->PlayAnimation(PlayerAnimationType::MoveFWD);
	}
	else
	{
		owner->PlayAnimation(PlayerAnimationType::Idle);
	}


	if (owner->IsGuardTrigger())
	{
		m_pMachine->ChangeState<PlayerGuardState>();
		return;
	}

	if (owner->IsJumpPressed())
	{
		m_pMachine->ChangeState<PlayerJumpStartState>();
		return;
	}

	if (owner->IsAttackPressed())
	{
		m_pMachine->ChangeState<PlayerAttackState>();
		return;
	}

	if (owner->GetStateType() != PlayerStateType::AttackState)
	{
		if (owner->IsAttackLongPressed())
		{
			m_pMachine->ChangeState<PlayerChargeAttackState>();
		}
	}
}

void PlayerNormalState::OnExit(Player* owner)
{

}
