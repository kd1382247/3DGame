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

	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }

	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }

	template<class T>
	void ChangeState()
	{
		m_stateMachine.ChangeState(*this, std::make_unique<T>());
	}

	// 各アクションボタン
	bool IsAttackButton()const      {return m_attackButton;}
	bool IsJumpButton()const        {return m_jumpButton;}
	bool IsGuardTrigger()const      { return m_guardTrigger; }
	bool IsSpeciaMovelButton()const     { return m_specialMoveButton;}


	// 移動フラグ
	bool IsMoving()const            { return m_moveFlg; }

	// アニメーションが終わったかどうかを返す
	bool IsAnimationFinished()const { return m_animation.IsFinished(); }


	// 攻撃
	void StartAttack();
	void EntAttack();
	// コンボ状態でアニメーションタイプを返す
	PlayerAnimationType GetAttackAnimation()const;

	// ジャンプ
	void StartJump();

	// 必殺技
	void StartSpecialMove();
	void EndSpecialMove();

	// ガード
	bool IsGuardCancel()const { return m_guardState == GuardState::GuardCancel; }
	bool IsGuardHitOrParyy()const { return m_guardState == GuardState::GuardHit || m_guardState == GuardState::Parry; }
	void ResetGuardState() { m_guardState = GuardState::Guard; }
	// ガード状態でアニメーションタイプを返す
	PlayerAnimationType GetGuardAnimation()const;

	// ガード中のパリィ入力
	void UpdateParryInput();

	// アニメーション
	void PlayAnimation(PlayerAnimationType type);

	// 通常の移動
	void UpdateMove();
	// 通常攻撃の移動
	void UpdateAttackMove();
	// 必殺技の移動
	void UpdateSpecialMove();

	enum class AttackType
	{
		None,
		NormalAttack,
		SpecialMove
	};

	void UpdateAttackCollision(const AttackType type);

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

	void UpdateSpecialMoveInput();

	void UpdateComboInput();

	
	void UpdateGravity();

	// 攻撃時のキャラの向き
	void FacingDirectionToCamera();

	// 状態を更新
	void UpdateComboState();

	void ResetCombo();

	void UpdateAnimation();

	// 攻撃判定のタイミングをセット
	void SetAttackTiming();

	// 必殺技の当たり判定のタイミングをセット
	void SetSpecialMoveTiming();


	void ClearHitTargets();

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

	// 攻撃キー
	bool            m_attackButton = false;
	bool            m_prevAttackButton = false;
	bool            m_attackTrigger = false;

	// 攻撃コンボ
	AttackCombo     m_currentAttackCombo = AttackCombo::Attack1;
	AttackCombo     m_preAttackCombo = AttackCombo::Attack1;

	int             m_comboInputCnt = 0;
	bool            m_canCombo = false;
	
	// 必殺技キー
	bool            m_specialMoveButton=false;

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

	Math::Vector3  m_specialMoveDir = {};
};