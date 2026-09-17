#include "PlayerJumpStartState.h"

#include"PlayerJumpAirState.h"

#include"../../Player.h"

void PlayerJumpStartState::OnStart(Player* owner)
{

	owner->SetStateType(PlayerStateType::JumpState);
	owner->StartJump();
	owner->PlayAnimation(PlayerAnimationType::JumpStart);
}

void PlayerJumpStartState::OnUpdate(Player * owner)
{
	owner->UpdateMove();

	if (owner->IsAnimationFinished())
	{
		m_pMachine->ChangeState<PlayerJumpAirState>();
	}
}

void PlayerJumpStartState::OnExit(Player * owner)
{

}
