#pragma once

#include"../Animation/PlayerAnimationType.h"
#include"../Parameter/PlayerParameter.h"
#include"../PlayerInput.h"

// ガードの状態(ガード中・解除)を持つクラス
// Playerのことは知らない。入力はPlayerInputから受け取る
class PlayerGuard
{
public:

	PlayerGuard() {}
	~PlayerGuard() {}

	void Init(const PlayerParameter::GuardParam& param) { m_pParam = &param; }

	void UpdateTimer(const float deltaTime);

	const PlayerParameter::GuardParam& GetParam()const { return *m_pParam; }

	PlayerAnimationType GetGuardAnimation()const;

	// ガードの状態を返す
	bool IsGuardHit()      const { return m_guardState == GuardState::GuardHit; }
	bool IsGuardBreak()    const { return m_guardState == GuardState::GuardBreak; }
	bool IsGuardStart()    const { return m_guardState == GuardState::GuardStart; }
	bool IsGuardEnd()      const { return m_guardState == GuardState::GuardEnd; }

	// ガード状態を元に戻す
	// パリィの後に再度ガード場合、Guard状態に戻るため
	// Guard中以外はStartGuardに戻す
	void ResetGuardState()
	{
		if (m_guardState != GuardState::Guard)
		{
			m_guardState = GuardState::GuardStart;
		}
	}

	// ガード中に攻撃を受けた際の処理
	void NotifyGuardHit();

	// 攻撃を防げるか
	bool IsDefend() const { return !IsGuardBreak() && !IsGuardStart() && !IsGuardEnd(); }

	// 状態を変更
	
	// ガード終了に移行
	void EndGuard() { m_guardState = GuardState::GuardEnd; }

	// ガード状態に移行
	void StartGuard() { m_guardState = GuardState::Guard; }


	// ガード中の移動アニメーションを返す
	PlayerAnimationType GetGuardMoveAnimation(const PlayerInput::MoveType moveType) const;

	// プレイヤーの正面、敵のノックバックの二つのベクトルで、ガード範囲内か判定
	bool IsInGuardRange(const Math::Vector3& forward, const Math::Vector3& knockBackDir)const;

private:

	enum class GuardState
	{
		GuardStart,
		Guard,
		GuardHit,
		GuardBreak,
		GuardEnd
	};

	// ガード状態
	GuardState      m_guardState = GuardState::GuardStart;

	// パラメータ(Playerが持つPlayerParameterの中身を参照する)
	const PlayerParameter::GuardParam* m_pParam = nullptr;

	int   m_guardHitCount = 0;
	float m_guardHitResetTimer = 0.0f;

};
