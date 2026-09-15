#pragma once
// 親クラス
#include "../CharacterBase.h"
// プレイヤー情報
#include"Animation/PlayerAnimationType.h"
#include"Animation/PlayerAnimation.h"
#include"Parameter/PlayerParameter.h"

#include"State/PlayerStateMachine.h"

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

	void AnimaFrame();

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

	//================================
	// 各アクションの入力
	//================================

	bool IsAttackPressed()		  const      {return m_playerAttack.IsAttackPressed();}
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

	bool HasNextCombo()           const      { return m_playerAttack.HasNextCombo(); }
	void NextCombo();
	void ResetCombo();
	void UpdateComboReception();
	void UpdateComboGrace();


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
	void UpdateMove();


	enum class AttackType
	{
		None,
		NormalAttack,
		SpecialMove
	};

	template<class T>
	void ChangeState()
	{
		m_stateMachine.ChangeState(*this, std::make_unique<T>());
	}



	void UpdateAttackCollision(const AttackType type);

	// 攻撃時のキャラの向き
	void FacingDirectionToCamera();


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

	float             m_animFrameCount = 0.0f;

	// カメラ
	std::weak_ptr<CameraBase> m_wpCamera;

	// アニメーションクラス
	PlayerAnimation           m_animation;

	// パラメータークラス
	PlayerParameter           m_parameter;

	// ステートマシン
	PlayerStateMachine        m_stateMachine;

	// 攻撃が当たった敵リスト
	std::vector<std::weak_ptr<EnemyBase>>m_hitTargets = {};

	// 一定時間で当たった敵のリストをクリア
	float                                m_hitCooldownTimer = 0.0f;
};