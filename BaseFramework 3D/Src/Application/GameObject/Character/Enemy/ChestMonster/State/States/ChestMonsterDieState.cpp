#include "ChestMonsterDieState.h"

#include"../../ChestMonster.h"

void ChestMonsterDieState::Enter(ChestMonster& chestMonster)
{
	chestMonster.PlayAnimation(ChestMonsterAnimationType::Die);
}

void ChestMonsterDieState::Update(ChestMonster& chestMonster)
{
	if (chestMonster.IsAnimationFinished())
	{
		chestMonster.Destroy();
	}
}

void ChestMonsterDieState::Exit(ChestMonster& chestMonster)
{

}
