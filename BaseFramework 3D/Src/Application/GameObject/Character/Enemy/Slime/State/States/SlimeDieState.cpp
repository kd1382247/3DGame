#include "SlimeDieState.h"

#include"../../Slime.h"

void SlimeDieState::OnStart(Slime* slime)
{
	slime->PlayAnimation(SlimeAnimationType::Die);
}

void SlimeDieState::OnUpdate(Slime * slime)
{

	// アニメーションが終わったかつ地面に着いていたら
	if (slime->IsAnimationFinished()&&slime->IsGrounded())
	{
		slime->Split();
		slime->CreateDeathSmoke();
		slime->Destroy();
	}
}

void SlimeDieState::OnExit(Slime * slime)
{

}
