#include "ChestMonsterNormalState.h"

#include"../../ChestMonster.h"

#include"ChestMonsterSpawnEnemyState.h"


void ChestMonsterNormalState::Enter(ChestMonster& chestMonster)
{
	chestMonster.PlayAnimation(ChestMonsterAnimationType::Idle);
}

void ChestMonsterNormalState::Update(ChestMonster& chestMonster)
{

	if (chestMonster.IsSpawnEnemy())
	{
		chestMonster.ChangeState<ChestMonsterSpawnEnemyState>();
	}

}

void ChestMonsterNormalState::Exit(ChestMonster& chestMonster)
{

}
