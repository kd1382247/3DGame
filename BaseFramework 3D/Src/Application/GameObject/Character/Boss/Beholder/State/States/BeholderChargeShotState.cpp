#include "BeholderChargeShotState.h"

#include"BeholderNormalState.h"

void BeholderChargeShotState::OnStart(Beholder* owner)
{
	owner->PlayAnimation(BeholderAnimationType::EnergyChargeST);

	owner->CreateChargeSparkBullet();
}

void BeholderChargeShotState::OnUpdate(Beholder * owner)
{

	owner->SetTargetDir();
	owner->UpdateFacingDirection();


	// アニメーションの切替
	if (owner->GetPlayAnimType() == BeholderAnimationType::EnergyChargeST)
	{
		if (owner->IsAnimationFinished())
		{
			owner->PlayAnimation(BeholderAnimationType::EnergyChargeRPT);
		}
	}

	if(!m_isFire)
	{
		// 待機状態になったら
		if (owner->IsStandbyChargeSparkBullet())
		{
			owner->PlayAnimation(BeholderAnimationType::EnergyShot);

			m_isFire = true;
		}
	}
	else
	{
		// アニメーションがある程度進んだら弾を発射
		if (owner->AnimProgress() >= 0.6)
		{
			owner->FireChargeSparkBullet();

			m_pMachine->ChangeState<BeholderNormalState>();
		}
	}

}

void BeholderChargeShotState::OnExit(Beholder * owner)
{
	owner->EndAttack();
}
