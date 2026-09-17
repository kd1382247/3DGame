#include "PlayerDieState.h"

#include"../../Player.h"

void PlayerDieState::OnStart(Player* owner)
{
	owner->SetStateType(PlayerStateType::DieState);
	owner->PlayAnimation(PlayerAnimationType::Die);
}

void PlayerDieState::OnUpdate(Player * owner)
{
	if (owner->IsAnimationFinished())
	{

	}
}

void PlayerDieState::OnExit(Player * owner)
{

}
