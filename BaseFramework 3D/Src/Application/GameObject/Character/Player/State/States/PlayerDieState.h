#pragma once

class Player;

#include"../../../StateMachine/StateBase.h"

class PlayerDieState :public StateBase<Player>
{
public:

	void OnStart(Player* owner)override;
	void OnUpdate(Player* owner)override;
	void OnExit(Player* owner)override;

private:

	// ゲームオーバーを知らせたか (死亡モーションが終わった時に1回だけ知らせる)
	bool m_isNotified = false;

};