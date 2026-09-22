#include "BeholderNormalState.h"

#include"../../Beholder.h"

void BeholderNormalState::OnStart(Beholder* beholder)
{
	beholder->PlayAnimation(BeholderAnimationType::Idle);
}

void BeholderNormalState::OnUpdate(Beholder* beholder)
{

}

void BeholderNormalState::OnExit(Beholder* beholder)
{

}
