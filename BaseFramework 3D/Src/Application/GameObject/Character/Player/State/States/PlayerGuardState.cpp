#include "PlayerGuardState.h"

#include"../../Player.h"

#include"PlayerNormalState.h"
#include"PlayerParryState.h"

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

	// ガード中にパリィの入力を受けたら、ステートを変える
	if (owner->GetInput().IsParryTrigger())
	{
		m_pMachine->ChangeState<PlayerParryState>();
		return;
	}


	// ガード解除
	if (guard.IsGuardCancel())
	{
		m_pMachine->ChangeState<PlayerNormalState>();
		return;
	}
	
	if (guard.IsGuardHit())
	{
		owner->PlayAnimation(PlayerAnimationType::DefendHit);

		if (owner->IsAnimationFinished())
		{
			guard.ResetGuardState();
			owner->PlayAnimation(PlayerAnimationType::Defend);
		}
	}
	

}

void PlayerGuardState::OnExit(Player * owner)
{

}
