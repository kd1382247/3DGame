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
		// 召喚の合図が出てから敵が増えて上限に達していた時は、モーションをせずにやめる
		// (時間は来たままなので、敵が減って空きができたらすぐ召喚する)
		if (!chestMonster->IsUnderEnemyLimit())
		{
			chestMonster->SetIsSpawnEnemy(false);
			return;
		}

		m_pMachine->ChangeState<ChestMonsterSpawnEnemyState>();
	}

}

void ChestMonsterNormalState::OnExit(ChestMonster* chestMonster)
{

}
