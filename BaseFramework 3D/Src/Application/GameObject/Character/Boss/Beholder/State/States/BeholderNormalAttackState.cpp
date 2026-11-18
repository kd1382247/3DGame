#include "BeholderNormalAttackState.h"

void BeholderNormalAttackState::OnStart(Beholder* owner)
{

}

void BeholderNormalAttackState::OnUpdate(Beholder * owner)
{
	owner->UpdateFacingDirection();




	owner->PlayAnimation(BeholderAnimationType::EnergyAttack);

}

void BeholderNormalAttackState::OnExit(Beholder * owner)
{

}
