#include "BombDieState.h"

#include"../../Bomb.h"

void BombDieState::OnStart(Bomb* bomb)
{
	bomb->PlayAnimation(BombAnimationType::Die);
	bomb->HideExplosionRange();
}

void BombDieState::OnUpdate(Bomb * bomb)
{
	if (bomb->IsAnimationFinished())
	{
		bomb->CreateDeathSmoke();
		bomb->Destroy();
	}
}

void BombDieState::OnExit(Bomb * bomb)
{

}
