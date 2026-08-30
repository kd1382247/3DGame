#include "PlayerJumpStartState.h"

#include"PlayerJumpAirState.h"

#include"../../Player.h"


void PlayerJumpStartState::Enter(Player& player)
{
	player.StartJump();
	player.PlayAnimation(PlayerAnimationType::JumpStart);
}

void PlayerJumpStartState::Update(Player & player)
{
	player.UpdateMove();

	if (player.IsAnimationFinished())
	{
		player.ChangeState<PlayerJumpAirState>();
	}
}

void PlayerJumpStartState::Exit(Player & player)
{
	
}
