#include "PlayerJumpLandState.h"

#include"PlayerNormalState.h"

#include"../../Player.h"

void PlayerJumpLandState::OnStart(Player* owner)
{
	owner->PlayAnimation(PlayerAnimationType::JumpLand);
}

void PlayerJumpLandState::OnUpdate(Player * owner)
{

	owner->UpdateMove();

	if (owner->IsAnimationFinished())
	{
		m_pMachine->ChangeState<PlayerNormalState>();
		return;
	}
}

void PlayerJumpLandState::OnExit(Player * owner)
{}
