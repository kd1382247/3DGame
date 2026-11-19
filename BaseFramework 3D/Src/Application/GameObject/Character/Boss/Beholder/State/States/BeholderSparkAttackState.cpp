#include "BeholderSparkAttackState.h"

#include"BeholderNormalState.h"

void BeholderSparkAttackState::OnStart(Beholder* owner)
{
	// 弾を作る
	owner->CreateSparkBullet();

	m_startFire = false;
}

void BeholderSparkAttackState::OnUpdate(Beholder * owner)
{
	owner->SetTargetDir();
	owner->UpdateFacingDirection();
	
	if (m_startFire)
	{
		if (owner->AnimProgress() >= 0.7f)
		{
			owner->FireSparkBullet();
		}

		// 発射アニメーションが終わったら飛ばす
		if (owner->IsAnimationFinished())
		{
			owner->FireSparkBullet();

			m_pMachine->ChangeState<BeholderNormalState>();
		}
	}
	else
	{
		// 弾が待機状態になったら
		if (owner->IsStandbySparkBullet())
		{
			owner->PlayAnimation(BeholderAnimationType::EnergyAttack);

			m_startFire = true;
		}
	}
}

void BeholderSparkAttackState::OnExit(Beholder * owner)
{
	owner->EndAttack();
}
