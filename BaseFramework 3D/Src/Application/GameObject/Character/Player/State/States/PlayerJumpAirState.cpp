#include "PlayerJumpAirState.h"

#include"PlayerJumpLandState.h"

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
		m_pMachine->ChangeState<PlayerJumpLandState>();
	}
}

void PlayerJumpAirState::OnExit(Player * owner)
{

}
