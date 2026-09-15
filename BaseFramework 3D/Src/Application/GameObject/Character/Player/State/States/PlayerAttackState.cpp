#include "PlayerAttackState.h"

#include"PlayerNormalState.h"

#include"../../Player.h"

void PlayerAttackState::Enter(Player& player)
{
	player.StartCurrentAttack();
}

void PlayerAttackState::Update(Player& player)
{

	player.AnimaFrame();
	player.UpdateAttackMove();
	// 当たり判定
	player.UpdateAttackCollision(Player::AttackType::NormalAttack);

	player.UpdateComboReception();

	if (player.IsAnimationFinished())
	{
		if (player.HasNextCombo())
		{
			player.NextCombo();
			player.StartCurrentAttack();
		}
		else
		{
			if (player.IsLastCombo())
			{
				player.ResetCombo();
			}
			else
			{
				player.StartComboGrace();

			}

			player.ChangeState<PlayerNormalState>();
		}

		return;
	}
	
}

void PlayerAttackState::Exit(Player & player)
{
	
}
