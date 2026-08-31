#include "CactasDamageState.h"

#include"../../Cactas.h"

#include"CactasNormalState.h"
#include"CactasDieState.h"

void CactasDamageState::Enter(Cactas& cactas)
{
	cactas.PlayAnimation(CactasAnimationType::GetHit);
}

void CactasDamageState::Update(Cactas & cactas)
{

	if (cactas.IsInOutro())
	{
		cactas.ChangeState<CactasDieState>();
		return;
	}

	if (cactas.IsAnimationFinished())
	{
		cactas.ChangeState<CactasNormalState>();
		cactas.SetDamaged(false);
	}
}

void CactasDamageState::Exit(Cactas & cactas)
{
	cactas.EndAttack();
}
