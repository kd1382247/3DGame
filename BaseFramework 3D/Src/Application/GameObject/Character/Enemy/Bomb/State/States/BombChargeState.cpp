#include "BombChargeState.h"

#include"../../Bomb.h"

#include"BombAttackState.h"

void BombChargeState::OnStart(Bomb* bomb)
{
	bomb->PlayAnimation(BombAnimationType::AttackCharge);

	bomb->StartCharge();

}

void BombChargeState::OnUpdate(Bomb * bomb)
{
	bomb->UpdateCharge();

	// ためている間から爆発範囲を表示し、プレイヤーに危険を知らせる
	bomb->ShowExplosionRange();

	if (bomb->IsChargeFinished())
	{
		m_pMachine->ChangeState<BombAttackState>();
		return;
	}
}

void BombChargeState::OnExit(Bomb * bomb)
{

}
