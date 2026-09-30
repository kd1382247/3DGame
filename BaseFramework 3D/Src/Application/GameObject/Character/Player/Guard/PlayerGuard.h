#pragma once

#include"../Animation/PlayerAnimationType.h"
#include"../PlayerInput.h"

// ガードの状態(ガード中・解除・パリィ)を持つクラス
// Playerのことは知らない。入力はPlayerInputから受け取る
class PlayerGuard
{
public:

	PlayerGuard(){}
	~PlayerGuard(){}

	// ガードボタンを押した瞬間に、ガード/解除を切り替える(毎フレーム呼ぶ)
	void Update(const PlayerInput& input);

	// ガード中に攻撃ボタンが押されたらパリィにする
	void UpdateParry(const PlayerInput& input);

	PlayerAnimationType GetGuardAnimation()const;

	// ガード関連
	bool IsGuardCancel()const { return m_guardState == GuardState::GuardCancel; }
	bool IsGuardHitOrParry()const { return m_guardState == GuardState::GuardHit || m_guardState == GuardState::Parry; }
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

};
