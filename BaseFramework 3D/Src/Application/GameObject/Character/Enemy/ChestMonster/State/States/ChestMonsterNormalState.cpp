#include "ChestMonsterNormalState.h"

#include"../../ChestMonster.h"


void ChestMonsterNormalState::Enter(ChestMonster& chestMonster)
{
	chestMonster.PlayAnimation(ChestMonsterAnimationType::Idle);
}

void ChestMonsterNormalState::Update(ChestMonster& chestMonster)
{




}

void ChestMonsterNormalState::Exit(ChestMonster& chestMonster)
{

}
