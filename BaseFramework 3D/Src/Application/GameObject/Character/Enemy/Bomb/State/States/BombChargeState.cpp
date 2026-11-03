#include "BombChargeState.h"

#include"../../Bomb.h"

#include"BombAttackState.h"

void BombChargeState::OnStart(Bomb* bomb)
{
	bomb->PlayAnimation(BombAnimationType::AttackCharge);

	bomb->StartCharge();
	// ためている間から爆発範囲を表示し、プレイヤーに危険を知らせる
	bomb->CreateExplosionRange();

	KdEffekseerManager::GetInstance().Play("Enemy/Charge/Charge.efkefc", bomb->GetPos(),0.4f,0.6f);
	KdEffekseerManager::GetInstance().Play("Enemy/Charge/Charge2.efkefc", bomb->GetPos(),0.4f,0.6f);
	KdEffekseerManager::GetInstance().Play("Enemy/Charge/Charge3.efkefc", bomb->GetPos(),0.4f,0.6f);
}

void BombChargeState::OnUpdate(Bomb * bomb)
{
	bomb->UpdateCharge();

	bomb->UpdateExplosionRange();

	if (bomb->IsChargeFinished())
	{
		m_pMachine->ChangeState<BombAttackState>();
		return;
	}
}

void BombChargeState::OnExit(Bomb * bomb)
{

}
