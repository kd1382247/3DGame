#include "PlayerNormalState.h"


#include"../../Player.h"

#include"PlayerAttackState.h"
#include"PlayerChargeAttackState.h"
#include"PlayerJumpStartState.h"
#include"PlayerGuardState.h"


void PlayerNormalState::Enter(Player& player)
{
	player.SetStateType(PlayerStateType::NormalState);
}

void PlayerNormalState::Update(Player & player)
{
	player.UpdateMove();

	// 移動アニメーション
	if (player.IsMovePressed())
	{
		player.PlayAnimation(PlayerAnimationType::MoveFWD);
	}
	else
	{
		player.PlayAnimation(PlayerAnimationType::Idle);
	}

	// 状態遷移
	if (player.IsGuardTrigger())
	{
		player.ChangeState<PlayerGuardState>();
		return;
	}

	if (player.IsJumpPressed())
	{
		player.ChangeState<PlayerJumpStartState>();
		return;
	}

	if (player.IsAttackPressed())
	{
		player.ChangeState<PlayerAttackState>();
		return;
	}

	if (player.GetStateType() != PlayerStateType::AttackState)
	{
		if (player.IsAttackLongPressed())
		{
			player.ChangeState<PlayerChargeAttackState>();
		}
	}

}

void PlayerNormalState::Exit(Player & player)
{

}
