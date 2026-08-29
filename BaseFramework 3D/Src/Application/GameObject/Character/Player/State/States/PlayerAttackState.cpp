#include "PlayerAttackState.h"

#include"PlayerNormalState.h"

#include"../../Player.h"

void PlayerAttackState::Enter(Player& player)
{
	player.StartAttack();

	player.PlayAnimation(player.GetAttackAnimation());

}

void PlayerAttackState::Update(Player & player)
{
	if (player.IsAnimationFinished())
	{
		player.ChangeState<PlayerNormalState>();
		return;
	}
}

void PlayerAttackState::Exit(Player & player)
{
	player.EntAttack();
}
