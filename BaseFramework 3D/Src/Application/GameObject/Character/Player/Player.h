#pragma once
// 親クラス
#include "../CharacterBase.h"
// プレイヤー情報
#include"State/PlayerState.h"
#include"Animation/PlayerAnimationType.h"
#include"Animation/PlayerAnimation.h"
#include"Parameter/PlayerParameter.h"

#include"State/PlayerStateMachine.h"


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

	void DrawDebug()override;

	void DrawInspector()override;

	void OnHit(const AttackInfo attackInfo)override;

	template<class T>
	void ChangeState()
	{
		m_stateMachine.ChangeState(*this, std::make_unique<T>());
	}

	// 各アクションボタン
	bool IsAttackButton()const      {return m_attackButton;}
	bool IsJumpButton()const        {return m_jumpButton;}
	bool IsGuardTrigger()const      { return m_guardTrigger; }
	bool IsSpecialButton()const     { return m_specialButton;}


	// 移動フラグ
	bool IsMoving()const            { return m_moveFlg; }

	// アニメーションが終わったかどうかを返す
	bool IsAnimationFinished()const { return m_animation.IsFinished(); }


	// 攻撃
	void StartAttack();
	void EntAttack();

	// ジャンプ
	void StartJump();

	// アニメーション
	void PlayAnimation(PlayerAnimationType type);

	// コンボ状態
	PlayerAnimationType GetAttackAnimation()const;


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
	void UpdateSpecialMoveInput();

	void UpdateComboInput();

	void UpdateMove();

	void UpdateGravity();

	void UpdateSpecialMove();

	// 攻撃時のキャラの向き
	void AttackFacingDirection();

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

	// 攻撃判定のタイミングをセット
	void SetAttackTiming();

	// 必殺技の当たり判定のタイミングをセット
	void SetSpecialMoveTiming();


	void ClearHitTargets();

	void UpdateAttackCollision();
	bool IsAlreadyHit(const std::shared_ptr<EnemyBase>&enemy)const;

	void CreateSpecialMoveDir();

	void UpdateGroundCollision();

	// 攻撃判定のスフィアを作る
	DirectX::BoundingSphere CreateAttackSphere()const;

	// 必殺技判定のスフィアを作る
	DirectX::BoundingSphere CreateSpecialMoveSphere()const;

private:

	// カメラ
	std::weak_ptr<CameraBase>m_wpCamera;
	// アニメーションクラス
	PlayerAnimation     m_animation;
	// パラメータークラス
	PlayerParameter     m_parameter;

	// ステートマシン
	PlayerStateMachine  m_stateMachine;


	// プレイヤーの状態
	PlayerActionState   m_actionState = PlayerActionState::Normal;
	PlayerMoveState     m_moveState   = PlayerMoveState::Idle;

	// キャラが向いている方向
	UINT            m_dirType = 0;

	bool            m_moveFlg=false;
	const float     m_attackMoveSpeed = 0.08f;

	// 攻撃キー
	bool            m_attackButton = false;
	bool            m_prevAttackButton = false;
	bool            m_attackTrigger = false;

	// 攻撃コンボ
	AttackCombo     m_currentAttackCombo = AttackCombo::Attack1;

	AttackCombo     m_preAttackCombo = AttackCombo::Attack1;

	// 攻撃時のキャラの回転速度
	const float     m_attackTurnSpeed = 30.0f;

	int             m_comboInputCnt = 0;
	bool            m_canCombo = false;
	
	// 必殺技キー
	bool            m_specialButton=false;

	const float     m_specialSpeed = 0.3f;

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

	// 一定時間で当たった敵のリストをクリア
	float          m_hitCooldownTimer = 0.0f;
	const float    m_HitCooldownDuration = 5.0f;

	Math::Vector3  m_specialMoveDir = {};
};