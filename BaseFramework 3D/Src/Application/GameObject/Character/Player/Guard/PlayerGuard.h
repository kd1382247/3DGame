#pragma once

#include"../Animation/PlayerAnimationType.h"
#include"../Parameter/PlayerParameter.h"
#include"../PlayerInput.h"

// ガードの状態(ガード中・解除)を持つクラス
// Playerのことは知らない。入力はPlayerInputから受け取る
class PlayerGuard
{
public:

	PlayerGuard(){}
	~PlayerGuard(){}

	void Init(const PlayerParameter::GuardParam& param) { m_pParam = &param; }

	// ガードボタンを押した瞬間に、ガード/解除を切り替える(毎フレーム呼ぶ)
	void Update(const PlayerInput& input);

	void UpdateTimer(const float deltaTime);

	const PlayerParameter::GuardParam& GetParam()const { return *m_pParam; }

	PlayerAnimationType GetGuardAnimation()const;

	// ガード関連
	bool IsGuardHit()       const { return m_guardState == GuardState::GuardHit; }
	bool IsGuardCancel()    const { return m_guardState == GuardState::GuardCancel; }
	void ResetGuardState() { m_guardState = GuardState::Guard; }

	void NotifyGuardHit();

private:

	enum class GuardState
	{
		Guard,
		GuardHit,
		GuardCancel,
	};

	// ガード状態
	GuardState      m_guardState = GuardState::Guard;

	// パラメータ(Playerが持つPlayerParameterの中身を参照する)
	const PlayerParameter::GuardParam* m_pParam = nullptr;

	int   m_guardHitCount = 0;
	float m_guardHitResetTimer = 0.0f;

};
