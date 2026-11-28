#include "BombDieState.h"

#include"../../Bomb.h"

void BombDieState::OnStart(Bomb* bomb)
{
	bomb->PlayAnimation(BombAnimationType::Die);
	bomb->HideExplosionRange();
}

void BombDieState::OnUpdate(Bomb * bomb)
{
	// アニメーションが終わったかつ地面に着いていたら
	if (bomb->IsAnimationFinished()&&bomb->IsGrounded())
	{
		bomb->CreateDeathSmoke();
		bomb->Destroy();
	}
}

void BombDieState::OnExit(Bomb * bomb)
{

}
