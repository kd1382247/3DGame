#include "CactasDamageState.h"

#include"../../Cactas.h"

#include"CactasNormalState.h"

void CactasDamageState::Enter(Cactas& cactas)
{
	cactas.PlayAnimation(CactasAnimationType::GetHit);
}

void CactasDamageState::Update(Cactas & cactas)
{
	if (cactas.IsAnimationFinished())
	{
		cactas.ChangeState<CactasNormalState>();
	}
}

void CactasDamageState::Exit(Cactas & cactas)
{
	cactas.EndAttack();
}
