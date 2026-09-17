#include "CactasLaunchState.h"

#include"../../Cactas.h"

#include"CactasNormalState.h"

void CactasLaunchState::OnStart(Cactas* cactas)
{
	cactas->PlayAnimation(CactasAnimationType::Idle);
}

void CactasLaunchState::OnUpdate(Cactas * cactas)
{
	cactas->UpdateLaunch();

	if (!cactas->IsLaunch())
	{
		m_pMachine->ChangeState<CactasNormalState>();
	}
}

void CactasLaunchState::OnExit(Cactas * cactas)
{

}
