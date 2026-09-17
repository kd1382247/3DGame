#include "PlayerSpecialMoveState.h"

#include"../../Player.h"

#include"PlayerNormalState.h"

void PlayerSpecialMoveState::OnStart(Player* owner)
{
	owner->SetStateType(PlayerStateType::SpecialMoveState);
	owner->StartSpecialMove();
	owner->PlayAnimation(PlayerAnimationType::AttackSpin);
}

void PlayerSpecialMoveState::OnUpdate(Player * owner)
{
	owner->UpdateAttackFrame();
	owner->UpdateSpecialMove();

	// 当たり判定
	owner->UpdateAttackCollision(Player::AttackType::SpecialMove);

	if (owner->IsAnimationFinished())
	{
		m_pMachine->ChangeState<PlayerNormalState>();
		return;
	}
}

void PlayerSpecialMoveState::OnExit(Player * owner)
{
	owner->EndSpecialMove();
}
