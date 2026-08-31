#include "CactasAttackState.h"

#include"CactasNormalState.h"
#include"CactasDieState.h"
#include"CactasDamageState.h"

#include"../../Cactas.h"

void CactasAttackState::Enter(Cactas& cactas)
{
	cactas.StartAttack();
	cactas.PlayAnimation(CactasAnimationType::Attack);
}

void CactasAttackState::Update(Cactas & cactas)
{

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

	cactas.UpdateAttackCollision();

	if (cactas.IsAnimationFinished())
	{
		cactas.ChangeState<CactasNormalState>();
		return;
	}
}

void CactasAttackState::Exit(Cactas & cactas)
{
	cactas.EndAttack();
}
