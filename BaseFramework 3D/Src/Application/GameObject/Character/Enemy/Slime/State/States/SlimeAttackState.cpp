#include "SlimeAttackState.h"

#include"../../Slime.h"

#include"SlimeNormalState.h"

void SlimeAttackState::OnStart(Slime* slime)
{
	slime->StartAttack();
	slime->PlayAnimation(SlimeAnimationType::Attack);
}

void SlimeAttackState::OnUpdate(Slime * slime)
{

	slime->UpdateAttackCollision();

	if (slime->IsAnimationFinished())
	{
		m_pMachine->ChangeState<SlimeNormalState>();
		return;
	}

}

void SlimeAttackState::OnExit(Slime * slime)
{
	slime->EndAttack();
}
