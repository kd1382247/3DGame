#pragma once

#include"../Animation/PlayerAnimationType.h"

class Player;

class PlayerAttack
{
public:

	PlayerAttack() {}
	~PlayerAttack() {}

	// 入力チェック
	void UpdateAttackInput();

	// 攻撃ボタンが押されたかを返す
	bool IsAttackTrigger()           const { return m_isAttackTrigger; }

	void UpdateAttackMove(Player& player);

	void StartAttack();

	void SetAttackTiming(float& hitStart, float& hitEnd);

	//=================================
	// 攻撃コンボ関連
	//=================================
	
	bool IsLastCombo() const { return m_currentAttackCombo == AttackCombo::Attack3; }
	bool HasNextCombo() const { return m_nextAttack; }

	void StartComboGrace();
	void UpdateComboGrace();

	void ComboInputStartFrame();
	void UpdateComboReception(const float animFrameCount);


	void UpdateComboState();

	void ResetCombo();

	// コンボ状態でアニメーションタイプを返す
	PlayerAnimationType GetAttackAnimation()const;



	//=================================
	// チャージ攻撃関連
	//=================================

	bool IsAttackLongPressed() const { return m_isAttackLongPressed; }
	bool IsAttackDown() const { return m_preAttackPressed; }

	bool IsChargeComplete() const { return m_isChargeComplete; }


	void StartCharge();
	void EndCharge();
	void UpdateChargeTime();



	// チャージ中の移動アニメーションタイプを返す
	PlayerAnimationType GetChargeMoveAnimation(const Player& player)const;

private:

	void UpdateChargeAttackInput();

	enum class AttackCombo
	{
		Attack1,
		Attack2,
		Attack3
	};

	// 攻撃フラグ
	bool            m_isAttackTrigger = false;
	bool            m_preAttackPressed = false;

	// 攻撃コンボ
	AttackCombo     m_currentAttackCombo = AttackCombo::Attack1;

	float           m_comboInputStartFrame = 0.0f;
	bool            m_canCombo = false;
	bool            m_nextAttack = false;

	float           m_comboGraceTime = 0.0f;
	float           m_comboGraceDuration = 0.2f;
	bool            m_comboGraceActive = false;



	float           m_attackHeldSeconds = 0.0f;
	const float     m_longPressSeconds = 0.2f;
	bool            m_isAttackLongPressed = false;

	bool            m_isChargeComplete = false;


	float           m_chargeSeconds = 0.0f;
	const float     m_maxChargeSeconds = 0.5f;

};
