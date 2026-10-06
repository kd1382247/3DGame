#include "PlayerSpecialMoveState.h"

#include"../../Player.h"

#include"PlayerNormalState.h"

void PlayerSpecialMoveState::OnStart(Player* owner)
{
	owner->StartSpecialMove();
	owner->PlayAnimation(PlayerAnimationType::AttackSpin);

	// SEを流す
	KdAudioManager::Instance().PlaySE("Asset/Data/Sound/SE/Player/SpinAttack/SpinAttack.wav");
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
