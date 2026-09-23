#include "CactasDieState.h"

#include"../../Cactas.h"

void CactasDieState::OnStart(Cactas* cactas)
{
	cactas->PlayAnimation(CactasAnimationType::Die);
	
}

void CactasDieState::OnUpdate(Cactas * cactas)
{
	if (cactas->IsAnimationFinished())
	{
		cactas->CreateDeathSmoke();
		cactas->Destroy();
	}
}

void CactasDieState::OnExit(Cactas * cactas)
{

}
