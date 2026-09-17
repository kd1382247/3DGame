#include "CactasNormalState.h"

#include"../../Cactas.h"

#include"CactasAttackState.h"
#include"CactasLaunchState.h"

void CactasNormalState::OnStart(Cactas* cactas)
{
	cactas->PlayAnimation(CactasAnimationType::Idle);
}

void CactasNormalState::OnUpdate(Cactas * cactas)
{

	cactas->UpdateMove();

	if (cactas->IsLaunch())
	{
		m_pMachine->ChangeState<CactasLaunchState>();
		return;
	}

	if (cactas->IsAttack())
	{
		m_pMachine->ChangeState<CactasAttackState>();
		return;
	}
}

void CactasNormalState::OnExit(Cactas * cactas)
{

}
