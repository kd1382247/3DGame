#pragma once

class Player;

class PlayerMove
{
public:

	PlayerMove() {}
	~PlayerMove() {}

	void UpdateMove(Player& player);

	// 入力を受け付ける
	void UpdateMoveInput(Player& player);

	bool IsMovePressed()                 const { return m_movePressed; }

private:

	// 移動フラグ
	bool            m_movePressed = false;

};
