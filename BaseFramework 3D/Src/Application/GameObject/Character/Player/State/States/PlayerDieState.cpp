#include "PlayerDieState.h"

#include"../../Player.h"

void PlayerDieState::OnStart(Player* owner)
{
	owner->PlayAnimation(PlayerAnimationType::Die);
}

void PlayerDieState::OnUpdate(Player * owner)
{
	if (owner->IsAnimationFinished())
	{
		owner->PlayAnimation(PlayerAnimationType::DieStay);
	}
}

void PlayerDieState::OnExit(Player * owner)
{

}
