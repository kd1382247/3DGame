#pragma once

class Player;

#include"../../../StateMachine/StateBase.h"

class PlayerJumpStartState :public StateBase<Player>
{
public:

	void OnStart(Player* owner)override;
	void OnUpdate(Player* owner)override;
	void OnExit(Player* owner)override;

private:

	enum class JumpState
	{
		Start,// ジャンプスタート
		Rise  // 上昇中
	};

	JumpState m_jumpState = JumpState::Start;

};