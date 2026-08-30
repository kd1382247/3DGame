#include "PlayerSpecialMoveState.h"

#include"../../Player.h"

#include"PlayerNormalState.h"

void PlayerSpecialMoveState::Enter(Player& player)
{
	player.StartSpecialMove();
	player.PlayAnimation(PlayerAnimationType::AttackSpin);
}

void PlayerSpecialMoveState::Update(Player & player)
{
	player.UpdateSpecialMove();

	// 当たり判定
	player.UpdateAttackCollision(Player::AttackType::SpecialMove);


	if (player.IsAnimationFinished())
	{
		player.ChangeState<PlayerNormalState>();
		return;
	}
}

void PlayerSpecialMoveState::Exit(Player & player)
{
	player.EndSpecialMove();
}
	