#include "SlimeAttackState.h"

#include"../../Slime.h"

#include"SlimeNormalState.h"

void SlimeAttackState::Enter(Slime& slime)
{
	slime.StartAttack();
	slime.PlayAnimation(SlimeAnimationType::Attack);
}

void SlimeAttackState::Update(Slime & slime)
{

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
