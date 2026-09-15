#pragma once

#include"../Animation/PlayerAnimationType.h"

class Player;

class PlayerAttack
{
public:

	PlayerAttack() {}
	~PlayerAttack() {}

	void UpdateAttackInput();

	// 各アクションボタン
	bool IsAttackPressed()           const { return m_isAttackPressed; }

	void UpdateAttackMove(Player& player);

	void SetAttackTiming(float& hitStart, float& hitEnd) const;

	// 三段目のコンボの時
	bool IsLastCombo() const { return m_currentAttackCombo == AttackCombo::Attack3; }

	bool HasNextCombo() const { return m_nextAttack; }

	void StartAttack();

	void StartComboGrace();

	void UpdateComboReception(const float animFrameCount);

	void UpdateComboGrace();

	void ComboInputStartFrame();

	void UpdateComboState();
	void ResetCombo();
	


	// コンボ状態でアニメーションタイプを返す
	PlayerAnimationType GetAttackAnimation()const;


private:

	enum class AttackCombo
	{
		Attack1,
		Attack2,
		Attack3
	};

	// 攻撃フラグ
	bool            m_isAttackPressed = false;

	// 攻撃コンボ
	AttackCombo     m_currentAttackCombo = AttackCombo::Attack1;

	float           m_comboInputStartFrame = 0.0f;
	bool            m_canCombo = false;
	bool            m_nextAttack = false;

	float           m_comboGraceTime = 0.0f;
	float           m_comboGraceDuration = 0.2f;
	bool            m_comboGraceActive = false;

};
