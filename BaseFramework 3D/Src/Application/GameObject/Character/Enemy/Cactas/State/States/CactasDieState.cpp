#include "CactasDieState.h"

#include"../../Cactas.h"

void CactasDieState::OnStart(Cactas* cactas)
{
	cactas->PlayAnimation(CactasAnimationType::Die);
	
}

void CactasDieState::OnUpdate(Cactas * cactas)
{

	// アニメーションが終わったかつ地面に着いていたら
	if (cactas->IsAnimationFinished()&&cactas->IsGrounded())
	{
		cactas->CreateDeathSmoke();
		cactas->Destroy();
	}
}

void CactasDieState::OnExit(Cactas * cactas)
{

}
