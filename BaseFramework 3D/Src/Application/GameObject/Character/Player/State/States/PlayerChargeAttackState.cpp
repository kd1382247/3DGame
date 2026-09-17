#include "PlayerChargeAttackState.h"


#include"../../Player.h"

#include"PlayerNormalState.h"
#include"PlayerSpecialMoveState.h"


void PlayerChargeAttackState::Enter(Player& player)
{
	player.SetStateType(PlayerStateType::ChargeAttackState);
	player.StartCharge();
	player.PlayAnimation(player.GetChargeMoveAnimation());

}

void PlayerChargeAttackState::Update(Player& player)
{
	player.UpdateAttackMove();

	player.PlayAnimation(player.GetChargeMoveAnimation());

	player.UpdateChargeTime();

	if (player.IsChargeComplete())
	{

		auto spEffekseerObj = m_wpEffekseerObj.lock();

		if (!spEffekseerObj||!spEffekseerObj->IsPlaying())
		{
			m_wpEffekseerObj = KdEffekseerManager::GetInstance().
				Play("Player/Charge.efkefc", player.GetPos() + Math::Vector3(0.0f, 0.0f, 0.0f), 0.8f, 2.5f, false, 30, 90);
		}
	}

	EffectUpdate(player);

	if (!player.IsAttackDown())
	{
		if (player.IsChargeComplete())
		{
			player.ChangeState<PlayerSpecialMoveState>();
		}
		else
		{
			player.ChangeState<PlayerNormalState>();
		}
	}
}

void PlayerChargeAttackState::Exit(Player& player)
{
	player.EndCharge();
	player.ResetCombo();

	auto spEffekseerObj = m_wpEffekseerObj.lock();

	if (!spEffekseerObj)
	{
		return;
	}

	spEffekseerObj->StopEffect();
	m_wpEffekseerObj.reset();
}

void PlayerChargeAttackState::EffectUpdate(Player& player)
{
	auto spEffekseerObj = m_wpEffekseerObj.lock();

	if (!spEffekseerObj)
	{
		return;
	}

	spEffekseerObj->SetPos(player.GetPos());

}
