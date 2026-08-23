#pragma once
// 親クラス
#include "../CharacterBase.h"
// プレイヤー情報
#include"State/PlayerState.h"
#include"Animation/PlayerAnimation.h"
#include"Parameter/PlayerParameter.h"


class CameraBase;
class EnemyBase;

class Player : public CharacterBase
{
public:

	Player() {}
	~Player()			override {}

	void Init()			override;
	void Update()		override;
	void PostUpdate()   override;
	void SetUpReference()override;

	void DrawLit()override;

	void DrawInspector()override;


	void OnHit(const AttackInfo& attackInfo)override;

private:

	enum class AttackCombo
	{
		Attack1,
		Attack2,
		Attack3,
	};

	enum class GuardState
	{
		Guard,
		GuardHit,
		GuardCancel,
		Parry
	};

	// 入力を受け付ける
	void UpdateInput();
	void UpdateMoveInput();
	void UpdateJumpInput();
	void UpdateAttackInput();
	void UpdateGuardInput();
	void UpdateParryInput();

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

	void SetAttackTiming();


	void UpdateAttackCollision();
	bool IsAlreadyHit(const std::shared_ptr<EnemyBase>&enemy)const;


private:

	// カメラ
	std::weak_ptr<CameraBase>m_wpCamera;
	// アニメーションクラス
	PlayerAnimation     m_animation;
	// パラメータークラス
	PlayerParameter     m_parameter;


	// プレイヤーの状態
	PlayerActionState   m_actionState = PlayerActionState::Normal;
	PlayerMoveState     m_moveState   = PlayerMoveState::Idle;

	// キャラが向いている方向
	UINT            m_dirType = 0;

	bool            m_moveFlg=false;

	// 攻撃キー
	bool            m_attackButton = false;
	bool            m_prevAttackButton = false;
	bool            m_attackTrigger = false;

	// 攻撃コンボ
	AttackCombo     m_currentAttackCombo = AttackCombo::Attack1;

	AttackCombo     m_preAttackCombo = AttackCombo::Attack1;

	int             m_comboInputCnt = 0;
	bool            m_canCombo = false;

	// ガードキー
	bool            m_guardButton = false;
	bool            m_prevGuardButton = false;
	bool            m_guardTrigger = false;

	// ガード状態
	GuardState      m_guardState = GuardState::Guard;

	// ジャンプキー
	bool            m_jumpButton = false;

	// 攻撃が当たった敵リスト
	std::vector<std::weak_ptr<EnemyBase>>m_hitTargets = {};

};