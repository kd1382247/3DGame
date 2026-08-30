#include "PlayerNormalState.h"


#include"../../Player.h"

#include"PlayerAttackState.h"
#include"PlayerJumpStartState.h"
#include"PlayerGuardState.h"
#include"PlayerSpecialMoveState.h"

void PlayerNormalState::Enter(Player& player)
{
}

void PlayerNormalState::Update(Player & player)
{

	player.UpdateMove();

	// 移動アニメーション
	if (player.IsMoving())
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


	if (player.IsJumpButton())
	{
		player.ChangeState<PlayerJumpStartState>();
		return;
	}

	if (player.IsAttackButton())
	{
		player.ChangeState<PlayerAttackState>();
		return;
	}

	if (player.IsSpeciaMovelButton())
	{
		player.ChangeState<PlayerSpecialMoveState>();
		return;
	}
}

void PlayerNormalState::Exit(Player & player)
{

}
