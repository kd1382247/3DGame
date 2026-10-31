#include "PlayerGuardState.h"

#include"../../Player.h"

#include"PlayerNormalState.h"
#include"PlayerParryState.h"

namespace
{
	// 移動方向に合わせたガード中のアニメーションを返す
	PlayerAnimationType GetGuardAnimation(Player* owner)
	{
		return owner->GetGuard().GetGuardMoveAnimation(owner->GetInput().GetMoveType());
	}
}

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
	
	// ガード中の移動
	owner->UpdateAttackMove();

	// 足音再生
	owner->UpdateWalkSE(0.5);

	// ガード開始
	if (guard.IsGuardStart())
	{
		if (owner->IsAnimationFinished())
		{
			guard.StartGuard();
		}
		return;
	}
	
	// ガード終了
	if (guard.IsGuardEnd())
	{
		if (owner->IsAnimationFinished())
		{
			m_pMachine->ChangeState<PlayerNormalState>();
		}
		return;
	}

	// 移動方向に応じて歩行アニメーションを再生
	if(!guard.IsGuardBreak()&&!guard.IsGuardHit())
	{
		owner->PlayAnimation(GetGuardAnimation(owner));
	}


	// ガード中にパリィの入力を受けたら、ステートを変える
	if (owner->GetInput().IsParryTrigger()&&
		!guard.IsGuardHit()&&
		!guard.IsGuardBreak())
	{

		owner->GetParry().StartParry();
		m_pMachine->ChangeState<PlayerParryState>();

		return;
	}

	// ガード強制解除
	if (guard.IsGuardBreak())
	{
		owner->PlayAnimation(PlayerAnimationType::GuardBreak);

		if (owner->IsAnimationFinished())
		{
			m_pMachine->ChangeState<PlayerNormalState>();
		}

		return;
	}
	
	if (!guard.IsGuardHit() && !owner->GetInput().IsGuardDown())
	{
		guard.EndGuard();
		owner->PlayAnimation(guard.GetGuardAnimation());
	}

	if (guard.IsGuardHit())
	{

		owner->PlayAnimation(PlayerAnimationType::GuardHit);

		if (owner->IsAnimationFinished())
		{
			guard.StartGuard();
			owner->PlayAnimation(PlayerAnimationType::GuardIDLE);
		}
	}

}

void PlayerGuardState::OnExit(Player * owner)
{

}
