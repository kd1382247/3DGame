#include "PlayerParryState.h"

#include"../../Player.h"

#include"../../Parry/PlayerParry.h"

#include"PlayerNormalState.h"
#include"PlayerGuardState.h"

void PlayerParryState::OnStart(Player* owner)
{
	PlayerParry& parry = owner->GetParry();

	owner->PlayAnimation(PlayerAnimationType::Parry);

	m_isParryEnd = false;
}

void PlayerParryState::OnUpdate(Player* owner)
{
	// パリィ終了後にガードしない場合
	if (m_isParryEnd)
	{
		if(owner->IsAnimationFinished())
		{
			m_pMachine->ChangeState<PlayerNormalState>();
		}
		return;
	}


	if (owner->IsAnimationFinished())
	{
		// ガードのボタンが押されていたら
		if (owner->GetInput().IsGuardDown())
		{
			m_pMachine->ChangeState<PlayerGuardState>();
		}
		else
		{
			owner->PlayAnimation(PlayerAnimationType::GuardEnd);
			m_isParryEnd = true;
		}	
	}

}

void PlayerParryState::OnExit(Player* owner)
{
	PlayerParry& parry = owner->GetParry();

	parry.EndParry();

	parry.SetIsParrySuccess(false);

	parry.SetInvincibility(true);
}
