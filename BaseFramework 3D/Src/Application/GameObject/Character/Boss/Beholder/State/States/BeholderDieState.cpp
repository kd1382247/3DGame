#include "BeholderDieState.h"

void BeholderDieState::OnStart(Beholder* owner)
{
	// 目の追従を止める
	owner->SetEyeTracking(false);

	owner->PlayAnimation(BeholderAnimationType::Die);
}

void BeholderDieState::OnUpdate(Beholder* owner)
{

	owner->UpdateGravity();

	if (owner->IsAnimationFinished()&&owner->IsGrounded())
	{
		owner->Destroy();
		owner->PlayDeathSmoke();
	}
}

void BeholderDieState::OnExit(Beholder* owner)
{

}
