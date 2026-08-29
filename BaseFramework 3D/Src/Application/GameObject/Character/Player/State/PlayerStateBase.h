#pragma once

class Player;

class PlayerStateBase
{
public:

	PlayerStateBase(){}
	virtual ~PlayerStateBase() {}

	// このStateになった瞬間に1回だけ実行
	virtual void Enter(Player& player) {}

	// このState中、毎フレーム実行
	virtual void Update(Player& player) = 0;

	// このStateから抜ける瞬間に１回だけ
	virtual void Exit(Player&player){}

};

