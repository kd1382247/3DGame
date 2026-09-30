#include "PlayerChargeAttackState.h"


#include"../../Player.h"

#include"PlayerNormalState.h"
#include"PlayerSpecialMoveState.h"


namespace
{
	// 移動方向に合わせたチャージ中のアニメーションを返す
	PlayerAnimationType GetChargeAnimation(Player* owner)
	{
		return owner->GetAttack().GetChargeMoveAnimation(owner->GetInput().GetMoveType());
	}
}

void PlayerChargeAttackState::OnStart(Player* owner)
{
	PlayerAttack& attack = owner->GetAttack();

	attack.ResetCombo();
	attack.StartCharge();

	owner->PlayAnimation(GetChargeAnimation(owner));
}

void PlayerChargeAttackState::OnUpdate(Player * owner)
{
	PlayerAttack& attack = owner->GetAttack();

	owner->UpdateAttackMove();

	owner->PlayAnimation(GetChargeAnimation(owner));

	attack.UpdateChargeTime(owner->GetDeltaTime());

	if (attack.IsChargeComplete())
	{

		auto spEffekseerObj = m_wpEffekseerObj.lock();

		if (!spEffekseerObj || !spEffekseerObj->IsPlaying())
		{
			m_wpEffekseerObj = KdEffekseerManager::GetInstance().
				Play("Player/Charge.efkefc", owner->GetPos() + Math::Vector3(0.0f, 0.0f, 0.0f), 0.8f, 2.5f, false, 30, 90);
		}
	}

	EffectUpdate(owner);

	if (!owner->GetInput().IsAttackDown())
	{
		if (attack.IsChargeComplete())
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

	owner->GetAttack().EndCharge();
	owner->GetAttack().ResetCombo();

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
