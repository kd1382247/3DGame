#include "PlayerAttackState.h"

#include"PlayerNormalState.h"

#include"../../Player.h"

#include"PlayerChargeAttackState.h"

void PlayerAttackState::OnStart(Player* owner)
{

	owner->SetStateType(PlayerStateType::AttackState);
	owner->StartCurrentAttack();
}

void PlayerAttackState::OnUpdate(Player * owner)
{

	owner->UpdateAttackFrame();
	owner->UpdateAttackMove();
	// 当たり判定
	owner->UpdateAttackCollision(Player::AttackType::NormalAttack);

	owner->UpdateComboReception();

	if (owner->IsAnimationFinished())
	{

		if (owner->IsAttackLongPressed())
		{
			m_pMachine->ChangeState<PlayerChargeAttackState>();
			return;
		}

		if (owner->HasNextCombo())
		{
			owner->NextCombo();
			owner->StartCurrentAttack();
		}
		else
		{
			if (owner->IsLastCombo())
			{
				owner->ResetCombo();
			}
			else
			{
				owner->StartComboGrace();

			}

			m_pMachine->ChangeState<PlayerNormalState>();
		}

		return;
	}

}

void PlayerAttackState::OnExit(Player * owner)
{

}
