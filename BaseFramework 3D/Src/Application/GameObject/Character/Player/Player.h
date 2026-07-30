#pragma once

#include"State/PlayerState.h"

#include "../CharacterBase.h"

#include"Animation/PlayerAnimation.h"


class Player : public CharacterBase
{
public:

	Player() {}
	~Player()			override {}

	void Init()			override;
	void Update()		override;
	void PostUpdate()   override;
	void SetUpReference()override;

	std::string GetTypeName()const override
	{
		return "Player";
	}

private:


	enum class AttackCombo
	{
		Attack1,
		Attack2,
		Attack3,
	};

	void UpdateInput();
	void UpdateMove();
	void UpdateAttack();
	void UpdateActionState();
	void UpdateMoveState();

	void UpdateGroundState();

	void ChangeActionState(PlayerActionState _nextState);
	void EnterState(PlayerActionState _state);
	void ExitState(PlayerActionState _state);

	void UpdateAnimation();

private:


	// アニメーションクラス
	PlayerAnimation     m_animation;

	PlayerAnimationType m_animationType = PlayerAnimationType::Idle;
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
	float           m_attackInputCnt = 0;
	bool            m_canCombo = false;


	// ジャンプキー
	bool            m_jumpButton = false;
	bool            m_prevJumpButton = false;
	bool            m_jumpTrigger = false;

	float           m_moveSpeed = 0.15;
	float           m_turnSpeed = 10.0f;
	float           m_angle=0;


	float           m_frame=0;

	int hoge = 0;

};