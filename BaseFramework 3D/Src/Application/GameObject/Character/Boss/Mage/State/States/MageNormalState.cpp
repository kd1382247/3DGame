#include "MageNormalState.h"

#include"../../Mage.h"

#include"MageSummonState.h"
#include"MageTargetCircleState.h"
#include"MageForwardSectorState.h"
#include"MageBoltState.h"
#include"MageNovaCircleState.h"
#include"MageBeamState.h"

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

	//switch (mage->SelectAttackPattern())
	switch (MageAttackPattern::ForwardSector)
	{
	case MageAttackPattern::Summon:
		m_pMachine->ChangeState<MageSummonState>();
		break;
	case MageAttackPattern::TargetCircle:
		m_pMachine->ChangeState<MageTargetCircleState>();
		break;
	case MageAttackPattern::ForwardSector:
		m_pMachine->ChangeState<MageForwardSectorState>();
		break;
	case MageAttackPattern::Bolt:
		m_pMachine->ChangeState<MageBoltState>();
		break;
	case MageAttackPattern::NovaCircle:
		m_pMachine->ChangeState<MageNovaCircleState>();
		break;
	case MageAttackPattern::Beam:
		m_pMachine->ChangeState<MageBeamState>();
		break;
	}
}

void MageNormalState::OnExit(Mage* mage)
{

}
