#pragma once

#include"State/PlayerState.h"

#include "../CharacterBase.h"

#include"Animation/PlayerAnimation.h"

class CameraBase;

class Player : public CharacterBase
{
public:

	Player() {}
	~Player()			override {}

	void Init()			override;
	void Update()		override;
	void PostUpdate()   override;
	void SetUpReference()override;

	void SetCamera(const std::shared_ptr<CameraBase>& _camera)
	{
		m_wpCamera = _camera;
	}

private:


	enum class AttackCombo
	{
		Attack1,
		Attack2,
		Attack3,
	};

	// 入力を受け付ける
	void UpdateInput();
	void UpdateMoveInput();
	void UpdateJumpInput();
	void UpdateAttackInput();
	void UpdateComboInput();


	void UpdateMove();

	// 状態を更新
	void UpdateActionState();
	void UpdateMoveState();
	void UpdateComboState();
	void UpdateGroundState();

	void ResetCombo();

	// 状態を変更
	void ChangeActionState(PlayerActionState _nextState);
	void ExitState(PlayerActionState _state);
	void EnterState(PlayerActionState _state);

	void UpdateAnimation();




private:

	// カメラ
	std::weak_ptr<CameraBase>m_wpCamera;

	// アニメーションクラス
	PlayerAnimation     m_animation;

	PlayerActionState   m_actionState = PlayerActionState::Normal;
	PlayerMoveState     m_moveState   = PlayerMoveState::Idle;

	// キャラが向いている方向
	UINT            m_dirType = 0;

	bool            m_moveFlg=false;
	Math::Vector3   m_inputMoveDir = Math::Vector3::Zero;

	// 攻撃キー
	bool            m_attackButton = false;
	bool            m_prevAttackButton = false;
	bool            m_attackTrigger = false;

	// 攻撃コンボ
	AttackCombo     m_currentAttackCombo = AttackCombo::Attack1;
	AttackCombo     m_nextAttackCombo = m_currentAttackCombo;
	int             m_comboInputCnt = 0;
	bool            m_canCombo = false;

	// ジャンプキー
	bool            m_jumpButton = false;
	bool            m_prevJumpButton = false;
	bool            m_jumpTrigger = false;

	// 移動系
	float           m_moveSpeed = 0.15;
	float           m_turnSpeed = 12.0f;
	float           m_angle=0;

};