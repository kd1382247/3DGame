#pragma once

class Player;

class PlayerJump
{
public:

	void UpdateJumpInput(Player& player);

	bool IsJumpPressed() const { return m_jumpPressed; }

	void StartJump(Player& player);

private:
	// ジャンプフラグ
	bool m_jumpPressed = false;

};
