#include "BombDieState.h"

#include"../../Bomb.h"

void BombDieState::OnStart(Bomb* bomb)
{
	bomb->PlayAnimation(BombAnimationType::Die);
}

void BombDieState::OnUpdate(Bomb * bomb)
{
	if (bomb->IsAnimationFinished())
	{
		bomb->Destroy();
	}
}

void BombDieState::OnExit(Bomb * bomb)
{

}
