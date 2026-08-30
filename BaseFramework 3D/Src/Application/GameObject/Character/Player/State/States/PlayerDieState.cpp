#include "PlayerDieState.h"

#include"../../Player.h"

void PlayerDieState::Enter(Player& player)
{
	player.PlayAnimation(PlayerAnimationType::Die);
}

void PlayerDieState::Update(Player & player)
{
	
	if (player.IsAnimationFinished())
	{

	}

}

void PlayerDieState::Exit(Player & player)
{

}
