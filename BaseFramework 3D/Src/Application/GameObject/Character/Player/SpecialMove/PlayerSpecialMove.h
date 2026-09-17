#pragma once

class Player;

class PlayerSpecialMove
{
public:

	void UpdateSpecialMoveInput();

	void UpdateSpecialMove(Player& player);

	bool IsSpecialMovePressed() const { return m_isSpecialMovePressed; }

	void SetSpecialMoveTiming(float& hitStart, float& hitEnd);

	void CreateSpecialMoveDir(Player& player);

private:

	// 必殺技フラグ
	bool            m_isSpecialMovePressed = false;

	Math::Vector3   m_specialMoveDir = {};

};
