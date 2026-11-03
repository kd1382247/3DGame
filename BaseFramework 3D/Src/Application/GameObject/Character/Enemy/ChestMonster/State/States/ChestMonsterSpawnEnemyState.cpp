#include "ChestMonsterSpawnEnemyState.h"

#include"../../ChestMonster.h"

#include"ChestMonsterNormalState.h"

void ChestMonsterSpawnEnemyState::OnStart(ChestMonster* chestMonster)
{
	chestMonster->StartSpawnEnemy();
	chestMonster->PlayAnimation(ChestMonsterAnimationType::GenerateMinion);

	KdEffekseerManager::GetInstance().Play("Enemy/Smoke/Smoke.efkefc", chestMonster->GetPos()+Math::Vector3(0.0f,1.6f,0.0f), 1.0f, 0.4f, false);
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
