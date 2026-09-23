#include "ChestMonsterDieState.h"

#include"../../ChestMonster.h"

void ChestMonsterDieState::OnStart(ChestMonster* chestMonster)
{
	chestMonster->PlayAnimation(ChestMonsterAnimationType::Die);
}

void ChestMonsterDieState::OnUpdate(ChestMonster* chestMonster)
{
	if (chestMonster->IsAnimationFinished())
	{
		chestMonster->CreateDeathSmoke();
		chestMonster->Destroy();
	}
}

void ChestMonsterDieState::OnExit(ChestMonster* chestMonster)
{

}
