#include "CactasHitShakeState.h"

#include"../../Cactas.h"

#include"CactasDamageState.h"

void CactasHitShakeState::OnStart(Cactas* cactas)
{
	cactas->StartHitShake();
	cactas->PlayAnimation(CactasAnimationType::Idle);
}

void CactasHitShakeState::OnUpdate(Cactas* cactas)
{

	cactas->UpdateHitShake();

	if (!cactas->GetIsHitShake())
	{
		m_pMachine->ChangeState<CactasDamageState>();
	}

}

void CactasHitShakeState::OnExit(Cactas* cactas)
{
	cactas->EndHitShake();
}
