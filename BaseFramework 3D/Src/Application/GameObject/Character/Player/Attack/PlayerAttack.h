#pragma once

#include"../Animation/PlayerAnimationType.h"
#include"../Parameter/PlayerParameter.h"
#include"../PlayerActionTiming.h"
#include"../PlayerInput.h"

// 通常攻撃(コンボ)とチャージの状態を持つクラス
// Playerのことは知らない。必要な情報は引数で受け取る
class PlayerAttack
{
public:

	enum class AttackCombo
	{
		Attack1,
		Attack2,
		Attack3
	};

	// コンボ1段ぶんのデータ(段ごとの違いはこの構造体に集める)
	struct AttackData
	{
		// 再生するアニメーション
		PlayerAnimationType animation = PlayerAnimationType::Attack1;

		// 攻撃判定・トレイルのフレーム区間
		PlayerActionTiming  timing = {};

		// 次のコンボ入力を受け付け始めるフレーム
		float               comboInputStartFrame = 0.0f;
	};


	PlayerAttack() {}
	~PlayerAttack() {}

	// パラメータの参照を受け取る(使う側が持つ)
	void Init(const PlayerParameter::AttackParam& param) { m_pParam = &param; }

	//=================================
	// 通常攻撃
	//=================================

	// 攻撃開始時の初期化(コンボ入力の受付状態をリセットする)
	void StartAttack();

	// 現在のコンボ段のデータ
	const AttackData& GetCurrentAttackData() const;

	// 現在のコンボ段の攻撃ヒット時のパラメータ
	const PlayerParameter::HitParam& GetHitParam()const { return m_pParam->m_hitParam[static_cast<int>(m_currentAttackCombo)]; }

	// 攻撃中の移動スピード
	float GetMoveSpeed() const { return m_pParam->m_attackMoveSpeed; }


	//=================================
	// 攻撃コンボ関連
	//=================================

	bool IsLastCombo() const { return m_currentAttackCombo == AttackCombo::Attack3; }
	bool HasNextCombo() const { return m_nextAttack; }

	// 攻撃後の、次のコンボを受け付ける猶予時間を開始する
	void StartComboGrace();
	void UpdateComboGrace(const PlayerInput& input, const float deltaTime);

	// 攻撃中の、次のコンボ入力の受付
	void UpdateComboReception(const PlayerInput& input, const float animFrameCount);

	// 次のコンボ段へ進める
	void NextCombo();

	void ResetCombo();


	//=================================
	// チャージ攻撃関連
	//=================================

	bool IsChargeComplete() const { return m_isChargeComplete; }

	void StartCharge();
	void EndCharge();
	void UpdateChargeTime(const float deltaTime);

	// チャージ中の移動方向に合わせたアニメーションタイプを返す
	PlayerAnimationType GetChargeMoveAnimation(const PlayerInput::MoveType moveType) const;

private:

	// パラメータ(Playerが持つPlayerParameterの中身を参照する)
	const PlayerParameter::AttackParam* m_pParam = nullptr;

	// 攻撃コンボ
	AttackCombo     m_currentAttackCombo = AttackCombo::Attack1;

	float           m_comboInputStartFrame = 0.0f;
	bool            m_canCombo = false;
	bool            m_nextAttack = false;

	float           m_comboGraceTime = 0.0f;
	float           m_comboGraceDuration = 0.2f;
	bool            m_comboGraceActive = false;

	// チャージ
	bool            m_isChargeComplete = false;

	float           m_chargeSeconds = 0.0f;
	const float     m_maxChargeSeconds = 0.5f;

};
