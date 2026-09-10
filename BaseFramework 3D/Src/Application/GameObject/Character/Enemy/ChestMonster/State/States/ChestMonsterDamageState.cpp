#include "ChestMonsterDamageState.h"

#include"../../ChestMonster.h"

#include"ChestMonsterNormalState.h"

void ChestMonsterDamageState::Enter(ChestMonster& chestMonster)
{
	chestMonster.PlayAnimation(ChestMonsterAnimationType::GetHit);
}

void ChestMonsterDamageState::Update(ChestMonster& chestMonster)
{
	if (chestMonster.IsAnimationFinished())
	{
		chestMonster.ChangeState<ChestMonsterNormalState>();
	}
}

void ChestMonsterDamageState::Exit(ChestMonster& chestMonster)
{
	
}
