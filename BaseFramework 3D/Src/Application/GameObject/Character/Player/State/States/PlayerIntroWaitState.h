#pragma once

class Player;

#include"../../../StateMachine/StateBase.h"

// ボスの登場演出中の待機ステート(操作を受け付けず、その場で待つ)
class PlayerIntroWaitState :public StateBase<Player>
{
public:

	void OnStart(Player* owner)override;
	void OnUpdate(Player* owner)override;
	void OnExit(Player* owner)override;

};
