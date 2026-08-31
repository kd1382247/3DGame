#include "CactasLaunchState.h"

#include"../../Cactas.h"

#include"CactasNormalState.h"

void CactasLaunchState::Enter(Cactas& cactas)
{
	cactas.PlayAnimation(CactasAnimationType::Idle);
}

void CactasLaunchState::Update(Cactas & cactas)
{
	cactas.UpdateLaunch();

	if (!cactas.IsLaunch())
	{
		cactas.ChangeState<CactasNormalState>();
	}
}

void CactasLaunchState::Exit(Cactas & cactas)
{

}
