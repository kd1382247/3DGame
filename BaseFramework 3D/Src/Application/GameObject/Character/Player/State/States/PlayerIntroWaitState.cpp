#include "PlayerIntroWaitState.h"

#include"../../Player.h"

#include"PlayerNormalState.h"

#include"../../../../Gimmick/Room/RoomManager.h"

void PlayerIntroWaitState::OnStart(Player* owner)
{
	// 途中だったコンボを切る
	owner->GetAttack().ResetCombo();

	// 鳴っていた足音を止める
	owner->StopWalkSE();
}

void PlayerIntroWaitState::OnUpdate(Player* owner)
{
	if (owner->IsAnimationFinished()||owner->IsAnimationLoop())
	{
		owner->PlayAnimation(PlayerAnimationType::Idle);
	}

	// 演出が終わったら通常の操作に戻る
	if (!RoomManager::Instance().IsIntro() &&
		!RoomManager::Instance().IsBossDefeat())
	{
		m_pMachine->ChangeState<PlayerNormalState>();
		return;
	}
}

void PlayerIntroWaitState::OnExit(Player* owner)
{

}
