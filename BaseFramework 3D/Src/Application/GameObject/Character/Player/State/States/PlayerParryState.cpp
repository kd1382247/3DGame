#include "PlayerParryState.h"

#include"../../Player.h"

#include"../../Parry/PlayerParry.h"

#include"PlayerNormalState.h"
#include"PlayerGuardState.h"

void PlayerParryState::OnStart(Player* owner)
{
	PlayerParry& parry = owner->GetParry();
	
	parry.StartParry();

	owner->PlayAnimation(PlayerAnimationType::Parry);
}

void PlayerParryState::OnUpdate(Player* owner)
{

	PlayerParry& parry = owner->GetParry();

	// パリィの受付時間更新
	parry.UpdateParryWindow(owner->GetDeltaTime());

	if (owner->IsAnimationFinished())
	{

		// ガードのボタンが押されていたら
		if (owner->GetInput().IsGuardDown())
		{
			m_pMachine->ChangeState<PlayerGuardState>();
		}
		else
		{
			m_pMachine->ChangeState<PlayerNormalState>();
		}
		
	}
	

}

void PlayerParryState::OnExit(Player* owner)
{
	PlayerParry& parry = owner->GetParry();

	parry.SetIsParrySuccess(false);
}
