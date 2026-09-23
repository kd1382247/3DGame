#include "BombAttackState.h"

#include"../../Bomb.h"

void BombAttackState::OnStart(Bomb* bomb)
{
	bomb->PlayAnimation(BombAnimationType::Attack);
	bomb->Explode();
}

void BombAttackState::OnUpdate(Bomb * bomb)
{

	// ためている間から爆発範囲を表示し、プレイヤーに危険を知らせる
	bomb->UpdateExplosionRange();

	// 爆発(攻撃)モーションが終わったタイミングで、
	// 消えるのと同時に爆発を起こす(当たり判定は専用のExplosionオブジェクトが担当する)
	if (bomb->IsAnimationFinished())
	{
		// 表示していた爆発範囲を消す
		bomb->HideExplosionRange();
		bomb->Destroy();
		return;
	}
}

void BombAttackState::OnExit(Bomb * bomb)
{

}
