#include "PlayerChargeAttackState.h"


#include"../../Player.h"

#include"PlayerNormalState.h"
#include"PlayerSpecialMoveState.h"


void PlayerChargeAttackState::OnStart(Player* owner)
{
	owner->SetStateType(PlayerStateType::ChargeAttackState);
	owner->StartCharge();
	owner->PlayAnimation(owner->GetChargeMoveAnimation());
}

void PlayerChargeAttackState::OnUpdate(Player * owner)
{
	owner->UpdateAttackMove();

	owner->PlayAnimation(owner->GetChargeMoveAnimation());

	owner->UpdateChargeTime();

	if (owner->IsChargeComplete())
	{

		auto spEffekseerObj = m_wpEffekseerObj.lock();

		if (!spEffekseerObj || !spEffekseerObj->IsPlaying())
		{
			m_wpEffekseerObj = KdEffekseerManager::GetInstance().
				Play("Player/Charge.efkefc", owner->GetPos() + Math::Vector3(0.0f, 0.0f, 0.0f), 0.8f, 2.5f, false, 30, 90);
		}
	}

	EffectUpdate(owner);

	if (!owner->IsAttackDown())
	{
		if (owner->IsChargeComplete())
		{
			m_pMachine->ChangeState<PlayerSpecialMoveState>();
		}
		else
		{
			m_pMachine->ChangeState<PlayerNormalState>();
		}
	}
}

void PlayerChargeAttackState::OnExit(Player * owner)
{

	owner->EndCharge();
	owner->ResetCombo();

	auto spEffekseerObj = m_wpEffekseerObj.lock();

	if (!spEffekseerObj)
	{
		return;
	}

	spEffekseerObj->StopEffect();
	m_wpEffekseerObj.reset();
}

void PlayerChargeAttackState::EffectUpdate(Player* owner)
{
	auto spEffekseerObj = m_wpEffekseerObj.lock();

	if (!spEffekseerObj)
	{
		return;
	}

	spEffekseerObj->SetPos(owner->GetPos());

}
