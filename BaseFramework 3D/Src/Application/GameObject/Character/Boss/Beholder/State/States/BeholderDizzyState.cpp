#include "BeholderDizzyState.h"

void BeholderDizzyState::OnStart(Beholder* owner)
{
	// 目の追従を止める
	owner->SetEyeTracking(false);
}

void BeholderDizzyState::OnUpdate(Beholder * owner)
{

}

void BeholderDizzyState::OnExit(Beholder * owner)
{

	// 目の追従を始める
	owner->SetEyeTracking(true);
}
