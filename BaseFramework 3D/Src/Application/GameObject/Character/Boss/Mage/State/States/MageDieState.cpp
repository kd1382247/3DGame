#include "MageDieState.h"

#include"../../Mage.h"

#include"../../../../../Gimmick/Room/Room.h"
#include"../../../../../Gimmick/Room/RoomManager.h"


void MageDieState::OnStart(Mage* mage)
{
	m_isFinished = false;

	mage->PlayAnimation(MageAnimationType::Die);

	// 出ている攻撃を全部消す
	mage->CancelAllAttacks();

	// 部屋に、ボスを倒したことを知らせる
	// (部屋が撃破演出に入り、部屋の中の敵が、ボスと同時に全員倒される)
	auto room = RoomManager::Instance().GetCurrentRoom();

	if (room)
	{
		room->StartBossDefeat();
	}

	// 撃破演出を、死亡アニメーションと同時に始める
	mage->StartDefeatDirection();
}

void MageDieState::OnUpdate(Mage * mage)
{
	if (m_isFinished)
	{
		return;
	}

	// 撃破演出を進める
	mage->UpdateDefeatDirection();

	// やられモーションが終わるまで待つ
	if (!mage->IsAnimationFinished())
	{
		return;
	}

	m_isFinished = true;

	// 死亡アニメーションが終わったら、演出も終わる
	mage->EndDefeatDirection();

	mage->PlayDeathSmoke();
	mage->Destroy();

	// 部屋に、撃破演出が終わったことを知らせる(扉が開く演出へ進む)
	auto room = RoomManager::Instance().GetCurrentRoom();

	if (room)
	{
		room->FinishBossDefeat();
	}
}

void MageDieState::OnExit(Mage * mage)
{

}
