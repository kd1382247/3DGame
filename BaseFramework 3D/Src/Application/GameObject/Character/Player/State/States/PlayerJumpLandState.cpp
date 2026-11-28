#include "PlayerJumpLandState.h"

#include"PlayerNormalState.h"

#include"../../Player.h"

#include"PlayerAttackState.h"

void PlayerJumpLandState::OnStart(Player* owner)
{
	owner->PlayAnimation(PlayerAnimationType::JumpFall);

	m_jumpState = JumpState::Fall;
}

void PlayerJumpLandState::OnUpdate(Player* owner)
{
	owner->UpdateMove();

	const auto& input = owner->GetInput();

	switch (m_jumpState)
	{
	case PlayerJumpLandState::JumpState::Fall:

		if (owner->IsGrounded())
		{
			// 着地時に攻撃入力があれば
			if (input.IsAttackDown())
			{
				m_pMachine->ChangeState<PlayerAttackState>();
				return;
			}

			owner->PlayAnimation(PlayerAnimationType::JumpLand);

			m_jumpState = JumpState::Land;
		}

		break;
	case PlayerJumpLandState::JumpState::Land:

		if (owner->IsAnimationFinished())
		{
			// 着地モーションに攻撃入力をしたら
			if (input.IsAttackDown())
			{
				m_pMachine->ChangeState<PlayerAttackState>();
				return;
			}

			m_pMachine->ChangeState<PlayerNormalState>();
		}


		break;

	}
}

void PlayerJumpLandState::OnExit(Player* owner)
{

}
