#include "BeholderDieState.h"

void BeholderDieState::OnStart(Beholder* owner)
{
	// 目の追従を止める
	owner->SetEyeTracking(false);
}

void BeholderDieState::OnUpdate(Beholder* owner)
{

}

void BeholderDieState::OnExit(Beholder* owner)
{

}
