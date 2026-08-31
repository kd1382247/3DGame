#include "CactasDieState.h"

#include"../../Cactas.h"

void CactasDieState::Enter(Cactas& cactas)
{
	cactas.PlayAnimation(CactasAnimationType::Die);
}

void CactasDieState::Update(Cactas & cactas)
{
	if (cactas.IsAnimationFinished())
	{
		cactas.Destroy();
	}
}

void CactasDieState::Exit(Cactas & cactas)
{

}
