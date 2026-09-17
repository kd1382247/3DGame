#include "ChestMonsterSpawnEnemyState.h"

#include"../../ChestMonster.h"

#include"ChestMonsterNormalState.h"

void ChestMonsterSpawnEnemyState::OnStart(ChestMonster* chestMonster)
{
	chestMonster->StartSpawnEnemy();
	chestMonster->PlayAnimation(ChestMonsterAnimationType::GenerateMinion);
}

void ChestMonsterSpawnEnemyState::OnUpdate(ChestMonster* chestMonster)
{

	chestMonster->AnimFrame();

	chestMonster->UpdateSpawnEnemy();

	if (chestMonster->IsAnimationFinished())
	{
		m_pMachine->ChangeState<ChestMonsterNormalState>();
	}

}

void ChestMonsterSpawnEnemyState::OnExit(ChestMonster * chestMonster)
{
	chestMonster->EndSpawnEnemy();
}
