#include "PlayerSpinChargeState.h"

#include"../../Player.h"

#include"PlayerNormalState.h"
#include"PlayerSpinAttackState.h"

namespace
{
	// 移動方向に合わせたチャージ中のアニメーションを返す
	PlayerAnimationType GetChargeAnimation(Player* owner)
	{
		return owner->GetAttack().GetChargeMoveAnimation(owner->GetInput().GetMoveType());
	}
}

void PlayerSpinChargeState::OnStart(Player* owner)
{
	PlayerAttack& attack = owner->GetAttack();

	attack.ResetCombo();
	attack.StartCharge();

	owner->PlayAnimation(PlayerAnimationType::SpinChargeStart);

	m_isPlaySE = false;

	// チャージ開始フラグ
	m_spinChargeStart = true;
}

void PlayerSpinChargeState::OnUpdate(Player * owner)
{
	PlayerAttack& attack = owner->GetAttack();

	owner->UpdateAttackMove();

	// 足音再生
	owner->UpdateWalkSE(0.5);

	// チャージの構える
	if (m_spinChargeStart)
	{
		if (owner->IsAnimationFinished())
		{
			m_spinChargeStart = false;
		}

		return;
	}

	owner->PlayAnimation(GetChargeAnimation(owner));

	attack.UpdateChargeTime(owner->GetDeltaTime());

	if (attack.IsChargeComplete())
	{
		PlaySpinAttackChargeFX(owner->GetPos());
	}

	EffectUpdate(owner);

	if (!owner->GetInput().IsAttackDown())
	{
		if (attack.IsChargeComplete())
		{
			m_pMachine->ChangeState<PlayerSpinAttackState>();
		}
		else
		{
			m_pMachine->ChangeState<PlayerNormalState>();
		}
	}
}

void PlayerSpinChargeState::OnExit(Player * owner)
{

	owner->GetAttack().EndCharge();
	owner->GetAttack().ResetCombo();

	// チャージキープのSEを止める
	auto spSountInst = m_wpSoundInst.lock();
	if (spSountInst)
	{
		spSountInst->Stop();
	}

	auto spEffekseerObj = m_wpEffekseerObj.lock();

	if (!spEffekseerObj)
	{
		return;
	}

	spEffekseerObj->StopEffect();
	m_wpEffekseerObj.reset();

	
}

void PlayerSpinChargeState::PlaySpinAttackChargeFX(const Math::Vector3& effectPos)
{
	if (!m_isPlaySE)
	{
		m_isPlaySE = true;
		// SEを流す
		KdAudioManager::Instance().PlaySE("Asset/Data/Sound/SE/Player/AttackCharge/AttackCharge.wav");
		m_wpSoundInst = KdAudioManager::Instance().PlaySE("Asset/Data/Sound/SE/Player/AttackCharge/ChargeKeep.wav", true);
	}
	auto spEffekseerObj = m_wpEffekseerObj.lock();

	if (!spEffekseerObj || !spEffekseerObj->IsPlaying())
	{
		m_wpEffekseerObj = KdEffekseerManager::Instance().
			Play("Player/Charge/Charge.efkefc", effectPos, 0.8f, 2.5f, false, 30, 90);
	}
}

void PlayerSpinChargeState::EffectUpdate(Player* owner)
{
	auto spEffekseerObj = m_wpEffekseerObj.lock();

	if (!spEffekseerObj)
	{
		return;
	}

	spEffekseerObj->SetPos(owner->GetPos());

}
