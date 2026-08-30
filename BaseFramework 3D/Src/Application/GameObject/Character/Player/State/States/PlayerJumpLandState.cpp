#include "PlayerJumpLandState.h"

#include"PlayerNormalState.h"

#include"../../Player.h"

void PlayerJumpLandState::Enter(Player& player)
{
	player.PlayAnimation(PlayerAnimationType::JumpLand);
}

void PlayerJumpLandState::Update(Player & player)
{

	player.UpdateMove();

	if (player.IsAnimationFinished())
	{
		player.ChangeState<PlayerNormalState>();
		return;
	}
}

void PlayerJumpLandState::Exit(Player & player)
{

}
