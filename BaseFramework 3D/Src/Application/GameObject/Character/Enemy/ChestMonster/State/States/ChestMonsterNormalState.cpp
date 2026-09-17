#include "ChestMonsterNormalState.h"

#include"../../ChestMonster.h"

#include"ChestMonsterSpawnEnemyState.h"


void ChestMonsterNormalState::OnStart(ChestMonster* chestMonster)
{
	chestMonster->PlayAnimation(ChestMonsterAnimationType::Idle);
}

void ChestMonsterNormalState::OnUpdate(ChestMonster* chestMonster)
{

	if (chestMonster->IsSpawnEnemy())
	{
		m_pMachine->ChangeState<ChestMonsterSpawnEnemyState>();
	}

}

void ChestMonsterNormalState::OnExit(ChestMonster* chestMonster)
{

}
