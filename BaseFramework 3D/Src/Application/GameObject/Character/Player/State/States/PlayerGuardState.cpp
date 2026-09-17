#include "PlayerGuardState.h"

#include"../../Player.h"

#include"PlayerNormalState.h"

void PlayerGuardState::OnStart(Player* owner)
{

	owner->SetStateType(PlayerStateType::GuradState);
	owner->ResetGuardState();
	owner->PlayAnimation(owner->GetGuardAnimation());
}

void PlayerGuardState::OnUpdate(Player * owner)
{

	owner->UpdateAttackMove();

	owner->UpdateParryInput();

	// ガード解除
	if (owner->IsGuardCancel())
	{
		m_pMachine->ChangeState<PlayerNormalState>();
		return;
	}

	// Plaryy / GuardHit終了
	if (owner->IsGuardHitOrParyy())
	{
		owner->PlayAnimation(owner->GetGuardAnimation());

		if (owner->IsAnimationFinished())
		{
			// 再度ガード状態に戻す
			owner->ResetGuardState();
			owner->PlayAnimation(PlayerAnimationType::Defend);
		}
	}
}

void PlayerGuardState::OnExit(Player * owner)
{

}
