#include "CactasNormalState.h"

#include"../../Cactas.h"

#include"CactasAttackState.h"
#include"CactasLaunchState.h"
#include"CactasDamageState.h"
#include"CactasDieState.h"

void CactasNormalState::Enter(Cactas& cactas)
{
	cactas.PlayAnimation(CactasAnimationType::Idle);
}

void CactasNormalState::Update(Cactas & cactas)
{

	cactas.UpdateMove();

	if (cactas.IsInOutro())
	{
		cactas.ChangeState<CactasDieState>();
		return;
	}

	if (cactas.IsDamaged())
	{
		cactas.ChangeState<CactasDamageState>();
		return;
	}

	if (cactas.IsLaunch())
	{
		cactas.ChangeState<CactasLaunchState>();
		return;
	}

	if (cactas.IsAttack())
	{
		cactas.ChangeState<CactasAttackState>();
		return;
	}

}

void CactasNormalState::Exit(Cactas & cactas)
{

}
