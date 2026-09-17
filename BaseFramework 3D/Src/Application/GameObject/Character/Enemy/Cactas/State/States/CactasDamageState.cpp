#include "CactasDamageState.h"

#include"../../Cactas.h"

#include"CactasNormalState.h"

void CactasDamageState::OnStart(Cactas* cactas)
{
	cactas->PlayAnimation(CactasAnimationType::GetHit);
}

void CactasDamageState::OnUpdate(Cactas * cactas)
{
	if (cactas->IsAnimationFinished())
	{
		m_pMachine->ChangeState<CactasNormalState>();
	}
}

void CactasDamageState::OnExit(Cactas * cactas)
{
	cactas->EndAttack();
}
