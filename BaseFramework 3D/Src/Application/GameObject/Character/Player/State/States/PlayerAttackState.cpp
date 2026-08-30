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

	player.UpdateAttackMove();
	// 当たり判定
	player.UpdateAttackCollision(Player::AttackType::NormalAttack);

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
