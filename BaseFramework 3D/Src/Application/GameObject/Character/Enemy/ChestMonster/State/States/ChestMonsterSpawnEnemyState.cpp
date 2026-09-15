#include "ChestMonsterSpawnEnemyState.h"

#include"../../ChestMonster.h"

#include"ChestMonsterNormalState.h"

void ChestMonsterSpawnEnemyState::Enter(ChestMonster& chestMonster)
{
	chestMonster.StartSpawnEnemy();
	chestMonster.PlayAnimation(ChestMonsterAnimationType::GenerateMinion);
}

void ChestMonsterSpawnEnemyState::Update(ChestMonster& chestMonster)
{

	chestMonster.AnimFrame();

	chestMonster.UpdateSpawnEnemy();

	if (chestMonster.IsAnimationFinished())
	{
		chestMonster.ChangeState<ChestMonsterNormalState>();
	}

}

void ChestMonsterSpawnEnemyState::Exit(ChestMonster & chestMonster)
{
	chestMonster.EndSpawnEnemy();
}
