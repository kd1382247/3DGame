#include "PlayerJumpAirState.h"

#include"PlayerJumpLandState.h"

#include"../../Player.h"

void PlayerJumpAirState::Enter(Player& player)
{
	player.PlayAnimation(PlayerAnimationType::JumpAir);
}

void PlayerJumpAirState::Update(Player & player)
{
	player.UpdateMove();

	if (player.IsGrounded())
	{
		player.ChangeState<PlayerJumpLandState>();
	}
}

void PlayerJumpAirState::Exit(Player & player)
{

}
