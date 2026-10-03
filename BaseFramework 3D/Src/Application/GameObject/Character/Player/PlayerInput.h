#pragma once

// プレイヤーのキー入力を1か所で読み取るクラス
// 「今どのボタンが押されているか」だけを持ち、接地やステートなどのゲーム状態は一切知らない
// (キー割り当てを変えたい時・パッド対応する時は、このクラスだけ直せばよい)
class PlayerInput
{
public:

	// 移動入力の方向(移動アニメーションの選択に使う)
	enum class MoveType
	{
		IDLE,
		BWD,
		FWD,
		LFT,
		RGT
	};

	// 毎フレーム1回、ステートの更新より前に呼ぶ
	void Update(const float deltaTime)
	{
		UpdateMoveInput();
		UpdateAttackInput(deltaTime);
		UpdateGuardInput();
		UpdateJumpInput();
		UpdateParryInput();
	}

	//================================
	// 移動
	//================================

	// 入力方向(カメラ基準に回す前の生の向き)
	const Math::Vector3& GetMoveDir()  const { return m_moveDir; }
	MoveType             GetMoveType() const { return m_moveType; }
	bool                 IsMovePressed() const { return m_movePressed; }

	//================================
	// 攻撃(左クリック)
	//================================

	// 押した瞬間だけtrue
	bool IsAttackTrigger()     const { return m_attackTrigger; }
	// 押している間ずっとtrue
	bool IsAttackDown()        const { return m_attackDown; }
	// 一定時間押し続けたらtrue(チャージ攻撃の開始判定)
	bool IsAttackLongPressed() const { return m_attackLongPressed; }

	//================================
	// ガード(右クリック)
	//================================

	// 押した瞬間だけtrue
	bool IsGuardTrigger() const { return m_guardTrigger; }
	bool IsGuardDown()    const { return m_guardDown; }

	//================================
	// パリィ(左クリック)
	//================================

	// 押した瞬間だけtrue
	bool IsParryTrigger() const { return m_parryTrigger; }

	//================================
	// ジャンプ(スペース)
	//================================

	// 押している間ずっとtrue(接地しているかどうかは呼び出し側で見る)
	bool IsJumpDown() const { return m_jumpDown; }

private:

	void UpdateMoveInput()
	{
		m_moveDir = Math::Vector3::Zero;
		m_moveType = MoveType::IDLE;
		m_movePressed = false;

		if (GetAsyncKeyState('W') & 0x8000)
		{
			m_moveDir.z += 1.0f;
			m_moveType = MoveType::BWD;
		}
		if (GetAsyncKeyState('S') & 0x8000)
		{
			m_moveDir.z -= 1.0f;
			m_moveType = MoveType::FWD;
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			m_moveDir.x -= 1.0f;
			m_moveType = MoveType::LFT;
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			m_moveDir.x += 1.0f;
			m_moveType = MoveType::RGT;
		}

		if (m_moveDir != Math::Vector3::Zero)
		{
			m_movePressed = true;
		}
	}

	void UpdateAttackInput(const float deltaTime)
	{
		const bool currentAttackButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

		m_attackTrigger = currentAttackButton && !m_attackDown;
		m_attackDown = currentAttackButton;

		// 長押しの判定
		if (m_attackDown)
		{
			m_attackHeldSeconds += deltaTime;

			if (m_attackHeldSeconds >= m_longPressSeconds)
			{
				m_attackLongPressed = true;
			}
		}
		else
		{
			m_attackLongPressed = false;
			m_attackHeldSeconds = 0.0f;
		}
	}

	void UpdateGuardInput()
	{
		const bool currentGuardButton = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

		m_guardTrigger = currentGuardButton && !m_guardDown;
		m_guardDown = currentGuardButton;
	}

	void UpdateParryInput()
	{
		const bool currentParryButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

		m_parryTrigger = currentParryButton && !m_parryDown;
		m_parryDown = currentParryButton;
	}

	void UpdateJumpInput()
	{
		m_jumpDown = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
	}

private:

	// 移動
	Math::Vector3 m_moveDir = Math::Vector3::Zero;
	MoveType      m_moveType = MoveType::IDLE;
	bool          m_movePressed = false;

	// 攻撃
	bool          m_attackTrigger = false;
	bool          m_attackDown = false;
	bool          m_attackLongPressed = false;
	float         m_attackHeldSeconds = 0.0f;
	const float   m_longPressSeconds = 0.2f;

	// ガード
	bool          m_guardTrigger = false;
	bool          m_guardDown = false;

	// パリィ
	bool          m_parryTrigger = false;
	bool          m_parryDown = false;

	// ジャンプ
	bool          m_jumpDown = false;
};
