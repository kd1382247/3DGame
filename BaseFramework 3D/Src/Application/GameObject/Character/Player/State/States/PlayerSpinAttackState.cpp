#include "PlayerSpinAttackState.h"

#include"../../Player.h"

#include"PlayerNormalState.h"

void PlayerSpinAttackState::OnStart(Player* owner)
{
	owner->StartSpinAttack();
	owner->PlayAnimation(PlayerAnimationType::AttackSpin);

	// SEを流す
	KdAudioManager::Instance().PlaySE("Asset/Data/Sound/SE/Player/SpinAttack/SpinAttack.wav");
}

void PlayerSpinAttackState::OnUpdate(Player * owner)
{
	owner->UpdateAttackFrame();
	owner->UpdateSpinAttack();

	// 当たり判定
	owner->UpdateAttackCollision(Player::AttackType::SpinAttack);

	if (owner->IsAnimationFinished())
	{
		m_pMachine->ChangeState<PlayerNormalState>();
		return;
	}
}

void PlayerSpinAttackState::OnExit(Player * owner)
{
	owner->EndSpinAttack();
}
