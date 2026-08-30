#include "PlayerGuardState.h"

#include"../../Player.h"

#include"PlayerNormalState.h"

void PlayerGuardState::Enter(Player& player)
{
	player.ResetGuardState();
	player.PlayAnimation(player.GetGuardAnimation());
}

void PlayerGuardState::Update(Player & player)
{

	player.UpdateAttackMove();

	player.UpdateParryInput();

	// ガード解除
	if (player.IsGuardCancel())
	{
		player.ChangeState<PlayerNormalState>();
		return;
	}
	
	// Plaryy / GuardHit終了
	if (player.IsGuardHitOrParyy())
	{
		player.PlayAnimation(player.GetGuardAnimation());

		if (player.IsAnimationFinished())
		{
			// 再度ガード状態に戻す
			player.ResetGuardState();
			player.PlayAnimation(PlayerAnimationType::Defend);
		}
	}



}

void PlayerGuardState::Exit(Player & player)
{
	
}
