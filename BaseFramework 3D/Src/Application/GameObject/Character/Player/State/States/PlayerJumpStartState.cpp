#include "PlayerJumpStartState.h"


#include"../../Player.h"

#include"PlayerJumpLandState.h"

void PlayerJumpStartState::OnStart(Player* owner)
{
	owner->StartJump();
	owner->PlayAnimation(PlayerAnimationType::JumpStart);

	m_jumpState = JumpState::Start;
}

void PlayerJumpStartState::OnUpdate(Player * owner)
{
	owner->UpdateMove();

	const auto& input = owner->GetInput();

	switch (m_jumpState)
	{
	case PlayerJumpStartState::JumpState::Start:

		if (owner->IsAnimationFinished())
		{
			owner->PlayAnimation(PlayerAnimationType::JumpRise);
			m_jumpState = JumpState::Rise;
		}

		break;
	case PlayerJumpStartState::JumpState::Rise:

		if (owner->GetGravity()>0)
		{
			m_pMachine->ChangeState<PlayerJumpLandState>();
		}
		break;
	}
}

void PlayerJumpStartState::OnExit(Player * owner)
{

}
