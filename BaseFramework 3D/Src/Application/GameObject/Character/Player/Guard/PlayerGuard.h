#pragma once

#include"../Animation/PlayerAnimationType.h"

class Player;

class PlayerGuard
{
public:

	PlayerGuard(){}
	~PlayerGuard(){}

	void UpdateGuardInput();
	void UpdateParryInput();

	
	bool IsGuardTrigger() const { return m_guardTrigger; }

	PlayerAnimationType GetGuardAnimation()const;

	// ガード関連
	bool IsGuardCancel()const { return m_guardState == GuardState::GuardCancel; }
	bool IsGuardHitOrParyy()const { return m_guardState == GuardState::GuardHit || m_guardState == GuardState::Parry; }
	void ResetGuardState() { m_guardState = GuardState::Guard; }

private:

	enum class GuardState
	{
		Guard,
		GuardHit,
		GuardCancel,
		Parry
	};

	// ガード状態
	GuardState      m_guardState = GuardState::Guard;

	// ガードフラグ
	bool            m_prevGuardPressed = false;
	bool            m_guardTrigger = false;


};
