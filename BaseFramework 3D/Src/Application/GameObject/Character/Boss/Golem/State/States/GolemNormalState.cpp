#include "GolemNormalState.h"

#include"../../Golem.h"

void GolemNormalState::OnStart(Golem* golem)
{
	golem->PlayAnimation(GolemAnimationType::Idle);
}

void GolemNormalState::OnUpdate(Golem* golem)
{

}

void GolemNormalState::OnExit(Golem* golem)
{

}
