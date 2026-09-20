#pragma once
// 親クラス
#include "../CharacterBase.h"
// プレイヤー情報
#include"Animation/PlayerAnimationType.h"
#include"Animation/PlayerAnimation.h"
#include"Parameter/PlayerParameter.h"

#include"../StateMachine/StateMachine.h"
#include"State/PlayerStateType.h"

#include"Move/PlayerMove.h"
#include"Attack/PlayerAttack.h"
#include"Guard/PlayerGuard.h"
#include"Jump/PlayerJump.h"
#include"SpecialMove/PlayerSpecialMove.h"


class CameraBase;
class EnemyBase;

class Player : public CharacterBase
{
public:


	Player() {}
	~Player()				override {}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;
	void SetUpReference()	override;

	void DrawLit()			override;

	void UpdateAttackFrame();

	void DrawDebug()		override;

	void DrawInspector()	override;

	void OnHit(const AttackInfo attackInfo);

	//================================
	// パラメータのゲッター
	//================================

	int GetMaxHP()              const override { return m_parameter.GetParam().m_maxHP; }
	float GetTurnSpeed()        const override { return m_parameter.GetParam().m_turnSpeed; }
	float GetMoveSpeed()        const          { return m_parameter.GetParam().m_moveSpeed; }
	float GetAttackMoveSpeed()  const          { return m_parameter.GetParam().m_attackMoveSpeed; }
	float GetJumpPower()        const          { return m_parameter.GetParam().m_jumpPow; }
	float GetSpecialMoveSpeed() const          { return m_parameter.GetParam().m_specialMoveSpeed; }
	float GetSpecialAttackPower()const         { return m_parameter.GetParam().m_specialAttackPower; }

	//================================
	// 各アクションの入力
	//================================

	bool IsAttackPressed()		  const      {return m_playerAttack.IsAttackTrigger();}
	bool IsJumpPressed()          const      { return m_playerJump.IsJumpPressed(); }
	bool IsSpecialMovePressed()    const      { return m_playerSpecialMove.IsSpecialMovePressed();}
	bool IsMovePressed()          const      { return  m_playerMove.IsMovePressed(); }
	bool IsGuardTrigger()         const      { return m_playerGuard.IsGuardTrigger(); }
	void UpdateParryInput();

	//================================
	// 通常攻撃
	//================================

	// 攻撃開始時の初期化
	void StartCurrentAttack();

	// 通常攻撃の移動
	void UpdateAttackMove();

	bool IsLastCombo()const { return m_playerAttack.IsLastCombo(); }

	// コンボの受付開始
	void StartComboGrace();

	bool HasNextCombo()const      { return m_playerAttack.HasNextCombo(); }
	void NextCombo();
	void ResetCombo();
	void UpdateComboReception();
	void UpdateComboGrace();

	//================================
	// チャージ攻撃
	//================================


	bool IsAttackLongPressed() const { return m_playerAttack.IsAttackLongPressed(); }
	bool IsAttackDown() const { return m_playerAttack.IsAttackDown(); }
	void StartCharge() { m_playerAttack.StartCharge(); }
	void EndCharge() { m_playerAttack.EndCharge(); }
	bool IsChargeComplete() const { return m_playerAttack.IsChargeComplete(); }
	void UpdateChargeTime() { m_playerAttack.UpdateChargeTime(); }
	PlayerAnimationType GetChargeMoveAnimation()const;


	//================================
	// ジャンプ
	//================================
	void StartJump();

	//================================
	// 必殺技
	//================================
	void StartSpecialMove();
	void EndSpecialMove();

	void UpdateSpecialMove();

	//================================
	// ガード
	//================================
	bool IsGuardCancel()      const { return m_playerGuard.IsGuardCancel(); }
	bool IsGuardHitOrParyy()  const { return m_playerGuard.IsGuardHitOrParyy(); }
	void ResetGuardState()          { m_playerGuard.ResetGuardState(); }

	// ガード状態でアニメーションタイプを返す
	PlayerAnimationType GetGuardAnimation()const { return m_playerGuard.GetGuardAnimation(); }

	//================================
	// アニメーション
	//================================
	void PlayAnimation(PlayerAnimationType type);
	
	// アニメーションが終わったかどうかを返す
	bool IsAnimationFinished()const { return m_animation.IsFinished(); }

	//================================
	// 通常移動
	//================================

	enum class MoveType
	{
		IDLE,
		BWD,
		FWD,
		LFT,
		RGT
	};

	void SetMoveType(const MoveType type) { m_moveType=type; }
	MoveType GetMoveType() const { return m_moveType; }

	void UpdateMove();


	enum class AttackType
	{
		None,
		NormalAttack,
		SpecialMove
	};

	void SetStateType(PlayerStateType type);
	PlayerStateType GetStateType() const { return m_playerStateType; }

	void UpdateAttackCollision(const AttackType type);

	// 攻撃時のキャラの向き
	void FacingDirectionToCamera();

	// カメラ基準の方向へ、指定した速度で水平移動を加える
	// (PlayerMove/PlayerAttackの移動処理で共通して使う)
	void ApplyCameraRelativeMove(float speed);

	std::weak_ptr<CameraBase>GetCamera()const { return m_wpCamera; }

private:

	// 入力を受け付ける
	void UpdateInput();

	void UpdateGravity();

	// コンボ状態でアニメーションタイプを返す
	PlayerAnimationType GetAttackAnimation() const { return m_playerAttack.GetAttackAnimation(); }

	void UpdateAnimation();


	//================================
	// 当たり判定
	//================================

	void ClearHitTargets();

	bool IsAlreadyHit(const std::shared_ptr<EnemyBase>&enemy)const;

	// 攻撃判定のスフィアを作る
	DirectX::BoundingSphere CreateAttackSphere()             const;

	// 必殺技判定のスフィアを作る
	DirectX::BoundingSphere CreateSpecialMoveSphere()        const;

private:


	// 各アクションクラス
	PlayerMove        m_playerMove;
	PlayerAttack      m_playerAttack;
	PlayerSpecialMove m_playerSpecialMove;
	PlayerJump        m_playerJump;
	PlayerGuard       m_playerGuard;



	MoveType        m_moveType = MoveType::IDLE;

	// カメラ
	std::weak_ptr<CameraBase> m_wpCamera;

	// アニメーションクラス
	PlayerAnimation           m_animation;

	// パラメータークラス
	PlayerParameter           m_parameter;

	// ステートマシン
	StateMachine<Player>      m_stateMachine;

	PlayerStateType           m_playerStateType = PlayerStateType::NormalState;

	// 攻撃が当たった敵リスト
	std::vector<std::weak_ptr<EnemyBase>>m_hitTargets = {};

	// 一定時間で当たった敵のリストをクリア
	float                                m_hitCooldownTimer = 0.0f;
};
