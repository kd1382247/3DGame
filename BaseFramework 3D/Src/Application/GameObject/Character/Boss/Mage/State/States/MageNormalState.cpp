#include "MageNormalState.h"

#include"../../Mage.h"

#include"MageSummonState.h"
#include"MageMagicCircleState.h"
#include"MageForwardAreaState.h"
#include"MageBoltState.h"
#include"MageAllDirectionState.h"
#include"MageCloneState.h"

void MageNormalState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Idle);
}

void MageNormalState::OnUpdate(Mage* mage)
{
	if (!mage->IsAttack())
	{
		return;
	}

	switch (mage->SelectAttackPattern())
	{
	case MageAttackPattern::Summon:
		m_pMachine->ChangeState<MageSummonState>();
		break;
	case MageAttackPattern::MagicCircle:
		m_pMachine->ChangeState<MageMagicCircleState>();
		break;
	case MageAttackPattern::ForwardArea:
		m_pMachine->ChangeState<MageForwardAreaState>();
		break;
	case MageAttackPattern::Bolt:
		m_pMachine->ChangeState<MageBoltState>();
		break;
	case MageAttackPattern::AllDirection:
		m_pMachine->ChangeState<MageAllDirectionState>();
		break;
	case MageAttackPattern::Clone:
		m_pMachine->ChangeState<MageCloneState>();
		break;
	}
}

void MageNormalState::OnExit(Mage* mage)
{

}
