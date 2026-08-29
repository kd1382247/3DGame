#include "PlayerNormalState.h"


#include"../../Player.h"

#include"PlayerAttackState.h"
#include"PlayerJumpStartState.h"


void PlayerNormalState::Enter(Player& player)
{

}

void PlayerNormalState::Update(Player & player)
{

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
}

void PlayerNormalState::Exit(Player & player)
{

}
