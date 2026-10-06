#include "PlayerNormalState.h"


#include"../../Player.h"

#include"PlayerAttackState.h"
#include"PlayerChargeAttackState.h"
#include"PlayerJumpStartState.h"
#include"PlayerGuardState.h"

void PlayerNormalState::OnStart(Player* owner)
{

}

void PlayerNormalState::OnUpdate(Player* owner)
{
	const PlayerInput& input = owner->GetInput();

	owner->UpdateMove();

	// 足音再生
	owner->UpdateWalkSE(0.3);

	// 移動アニメーション
	if (input.IsMovePressed())
	{
		owner->PlayAnimation(PlayerAnimationType::MoveFWD);
	}
	else
	{
		owner->PlayAnimation(PlayerAnimationType::Idle);
	}


	if (input.IsGuardTrigger())
	{
		m_pMachine->ChangeState<PlayerGuardState>();
		return;
	}

	// ジャンプは地面にいる時だけ
	if (input.IsJumpDown() && owner->IsGrounded())
	{
		m_pMachine->ChangeState<PlayerJumpStartState>();
		return;
	}

	if (input.IsAttackTrigger())
	{
		m_pMachine->ChangeState<PlayerAttackState>();
		return;
	}

	if (input.IsAttackLongPressed())
	{
		m_pMachine->ChangeState<PlayerChargeAttackState>();
	}
}

void PlayerNormalState::OnExit(Player* owner)
{

}
