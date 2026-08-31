#include "SlimeAttackState.h"

#include"../../Slime.h"

#include"SlimeDamageState.h"
#include"SlimeNormalState.h"
#include"SlimeDieState.h"

void SlimeAttackState::Enter(Slime& slime)
{
	slime.StartAttack();
	slime.PlayAnimation(SlimeAnimationType::Attack);
}

void SlimeAttackState::Update(Slime & slime)
{

	if (slime.IsInOutro())
	{
		slime.ChangeState<SlimeDieState>();
		return;
	}

	if (slime.IsDamaged())
	{
		slime.ChangeState<SlimeDamageState>();
		return;
	}
	
	slime.UpdateAttackCollision();

	if (slime.IsAnimationFinished())
	{
		slime.ChangeState<SlimeNormalState>();
		return;
	}

}

void SlimeAttackState::Exit(Slime & slime)
{
	slime.EndAttack();
}
