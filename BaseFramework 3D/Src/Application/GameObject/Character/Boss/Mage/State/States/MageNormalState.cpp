#include "MageNormalState.h"

#include"../../Mage.h"

void MageNormalState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Idle);
}

void MageNormalState::OnUpdate(Mage* mage)
{

}

void MageNormalState::OnExit(Mage* mage)
{

}
