#include "PlayerJumpAirState.h"

#include"PlayerJumpLandState.h"

#include"PlayerAttackState.h"

#include"../../Player.h"

void PlayerJumpAirState::OnStart(Player* owner)
{
	owner->PlayAnimation(PlayerAnimationType::JumpAir);
}

void PlayerJumpAirState::OnUpdate(Player * owner)
{
	owner->UpdateMove();

	if (owner->IsGrounded())
	{

		if (owner->GetInput().IsAttackDown())
		{
			m_pMachine->ChangeState<PlayerAttackState>();
		}
		else
		{
			m_pMachine->ChangeState<PlayerJumpLandState>();
		}
	}
}

void PlayerJumpAirState::OnExit(Player * owner)
{

}
