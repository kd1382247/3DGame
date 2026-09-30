#include "PlayerGuardState.h"

#include"../../Player.h"

#include"PlayerNormalState.h"

void PlayerGuardState::OnStart(Player* owner)
{
	PlayerGuard& guard = owner->GetGuard();

	owner->GetAttack().ResetCombo();

	guard.ResetGuardState();
	owner->PlayAnimation(guard.GetGuardAnimation());
}

void PlayerGuardState::OnUpdate(Player * owner)
{
	PlayerGuard& guard = owner->GetGuard();

	owner->UpdateAttackMove();

	guard.UpdateParry(owner->GetInput());

	// ガード解除
	if (guard.IsGuardCancel())
	{
		m_pMachine->ChangeState<PlayerNormalState>();
		return;
	}

	// Parry / GuardHit終了
	if (guard.IsGuardHitOrParry())
	{
		owner->PlayAnimation(guard.GetGuardAnimation());

		if (owner->IsAnimationFinished())
		{
			// 再度ガード状態に戻す
			guard.ResetGuardState();
			owner->PlayAnimation(PlayerAnimationType::Defend);
		}
	}
}

void PlayerGuardState::OnExit(Player * owner)
{

}
