#include "BeholderChargeShotState.h"

void BeholderChargeShotState::OnStart(Beholder* owner)
{
	owner->PlayAnimation(BeholderAnimationType::EnergyChargeST);
}

void BeholderChargeShotState::OnUpdate(Beholder * owner)
{


}

void BeholderChargeShotState::OnExit(Beholder * owner)
{

}
