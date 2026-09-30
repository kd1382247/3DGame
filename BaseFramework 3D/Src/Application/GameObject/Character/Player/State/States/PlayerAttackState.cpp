#include "PlayerAttackState.h"

#include"PlayerNormalState.h"

#include"../../Player.h"

#include"PlayerChargeAttackState.h"

void PlayerAttackState::OnStart(Player* owner)
{
	owner->StartCurrentAttack();
}

void PlayerAttackState::OnUpdate(Player * owner)
{
	PlayerAttack& attack = owner->GetAttack();

	owner->UpdateAttackFrame();
	owner->UpdateAttackMove();
	// 当たり判定
	owner->UpdateAttackCollision(Player::AttackType::NormalAttack);

	attack.UpdateComboReception(owner->GetInput(), owner->GetAnimFrame());

	if (owner->IsAnimationFinished())
	{

		if (owner->GetInput().IsAttackLongPressed())
		{
			m_pMachine->ChangeState<PlayerChargeAttackState>();
			return;
		}

		if (attack.HasNextCombo())
		{
			attack.NextCombo();
			owner->StartCurrentAttack();
		}
		else
		{
			if (attack.IsLastCombo())
			{
				attack.ResetCombo();
			}
			else
			{
				attack.StartComboGrace();

			}

			m_pMachine->ChangeState<PlayerNormalState>();
		}

		return;
	}

}

void PlayerAttackState::OnExit(Player * owner)
{
	owner->EndAttack();
}
