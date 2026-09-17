#include "ChestMonsterDamageState.h"

#include"../../ChestMonster.h"

#include"ChestMonsterNormalState.h"

void ChestMonsterDamageState::OnStart(ChestMonster* chestMonster)
{
	chestMonster->PlayAnimation(ChestMonsterAnimationType::GetHit);
}

void ChestMonsterDamageState::OnUpdate(ChestMonster* chestMonster)
{
	if (chestMonster->IsAnimationFinished())
	{
		m_pMachine->ChangeState<ChestMonsterNormalState>();
	}
}

void ChestMonsterDamageState::OnExit(ChestMonster* chestMonster)
{

}
