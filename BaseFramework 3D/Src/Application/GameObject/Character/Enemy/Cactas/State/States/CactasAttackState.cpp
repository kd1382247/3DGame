#include "CactasAttackState.h"

#include"CactasNormalState.h"

#include"../../Cactas.h"

void CactasAttackState::OnStart(Cactas* cactas)
{
	cactas->StartAttack();
	cactas->PlayAnimation(CactasAnimationType::Attack);
}

void CactasAttackState::OnUpdate(Cactas * cactas)
{

	cactas->UpdateAttackCollision();

	if (cactas->IsAnimationFinished())
	{
		m_pMachine->ChangeState<CactasNormalState>();
		return;
	}
}

void CactasAttackState::OnExit(Cactas * cactas)
{
	cactas->EndAttack();
}
