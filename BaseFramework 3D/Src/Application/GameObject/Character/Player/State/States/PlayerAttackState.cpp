#include "PlayerAttackState.h"

#include"PlayerNormalState.h"

#include"../../Player.h"

#include"PlayerChargeAttackState.h"

void PlayerAttackState::Enter(Player& player)
{

	player.SetStateType(PlayerStateType::AttackState);
	player.StartCurrentAttack();
}

void PlayerAttackState::Update(Player& player)
{

	player.UpdateAttackFrame();
	player.UpdateAttackMove();
	// 当たり判定
	player.UpdateAttackCollision(Player::AttackType::NormalAttack);

	player.UpdateComboReception();

	if (player.IsAnimationFinished())
	{

		if (player.IsAttackLongPressed())
		{
			player.ChangeState<PlayerChargeAttackState>();
			return;
		}

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
