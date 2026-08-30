#include "PlayerDamageState.h"

#include"../../Player.h"

#include"PlayerNormalState.h"

void PlayerDamageState::Enter(Player& player)
{
	player.PlayAnimation(PlayerAnimationType::GetHit);
}

void PlayerDamageState::Update(Player & player)
{

	player.UpdateMove();

	if (player.IsAnimationFinished())
	{
		player.ChangeState<PlayerNormalState>();
	}
}

void PlayerDamageState::Exit(Player & plaer)
{

}
