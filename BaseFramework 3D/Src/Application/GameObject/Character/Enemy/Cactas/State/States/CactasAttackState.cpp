#include "CactasAttackState.h"

#include"CactasNormalState.h"

#include"../../Cactas.h"

void CactasAttackState::Enter(Cactas& cactas)
{
	cactas.StartAttack();
	cactas.PlayAnimation(CactasAnimationType::Attack);
}

void CactasAttackState::Update(Cactas & cactas)
{

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
