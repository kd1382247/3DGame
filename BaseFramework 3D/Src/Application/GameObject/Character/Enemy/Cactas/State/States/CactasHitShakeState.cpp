#include "CactasHitShakeState.h"

#include"../../Cactas.h"

#include"CactasDamageState.h"

void CactasHitShakeState::Enter(Cactas& cactas)
{
	cactas.StartHitShake();
	cactas.PlayAnimation(CactasAnimationType::Idle);
}

void CactasHitShakeState::Update(Cactas& cactas)
{

	cactas.UpdateHitShake();

	if (!cactas.GetIsHitShake())
	{
		cactas.ChangeState<CactasDamageState>();
	}

}

void CactasHitShakeState::Exit(Cactas& cactas)
{
	cactas.EndHitShake();
}
