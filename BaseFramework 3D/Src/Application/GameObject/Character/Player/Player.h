#pragma once
// 親クラス
#include "../CharacterBase.h"
// プレイヤー情報
#include"Animation/PlayerAnimationType.h"
#include"Animation/PlayerAnimation.h"
#include"Parameter/PlayerParameter.h"

#include"../StateMachine/StateMachine.h"

#include"PlayerInput.h"
#include"PlayerActionTiming.h"
#include"Attack/PlayerAttack.h"
#include"Attack/PlayerHitChecker.h"
#include"Guard/PlayerGuard.h"
#include"SpecialMove/PlayerSpecialMove.h"
#include"SwordTrail/PlayerSwordTrail.h"


class CameraBase;
class EnemyBase;

// Playerの役割
//   ・各部品(入力・攻撃・ガード・必殺技・アニメーション・パラメータ)の持ち主
//   ・部品を更新する順番を決める(Update)
//   ・体そのものの操作(カメラ基準の移動・ジャンプ・重力・攻撃判定の実行・被弾)
//
// 部品の状態を知りたい時は、Playerを経由せず GetInput() / GetAttack() / GetGuard() で部品に直接聞く。
// 部品はPlayerのことを知らない(依存は Player → 部品 の一方向)
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
	void DrawEffect()       override;

	void DrawDebug()		override;

	void DrawInspector()	override;

	void OnHit(const AttackInfo attackInfo) override;

	//================================
	// CharacterBaseが要求するパラメータ
	//================================

	float GetTurnSpeed() const override { return m_parameter.GetBody().m_turnSpeed; }
	float GetMoveSpeed() const override { return m_parameter.GetMove().m_moveSpeed; }
	float GetGravityAcceleration() const override { return m_parameter.GetBody().m_gravityAcceleration; }

	//================================
	// 部品へのアクセス(ステートが状態を直接聞くために使う)
	//================================

	const PlayerInput& GetInput()  const { return m_input; }
	PlayerAttack&      GetAttack()       { return m_playerAttack; }
	PlayerGuard&       GetGuard()        { return m_playerGuard; }

	//================================
	// アクション(体の操作)
	//================================

	// 通常移動(カメラ基準で移動して、進む向きへ向く)
	void UpdateMove();

	// 攻撃中・ガード中の移動(カメラ基準で移動して、カメラの向きへ向く)
	void UpdateAttackMove();

	// 通常攻撃の開始 / 終了
	void StartCurrentAttack();
	void EndAttack();

	// アニメーションの経過フレームを進める(攻撃判定・トレイルの区間指定に使う)
	void UpdateAttackFrame();

	enum class AttackType
	{
		None,
		NormalAttack,
		SpecialMove
	};

	// 攻撃判定を実行する(攻撃判定のフレーム区間の中だけ有効)
	void UpdateAttackCollision(const AttackType type);

	// ジャンプの開始
	void StartJump();

	// 必殺技の開始 / 更新 / 終了
	void StartSpecialMove();
	void UpdateSpecialMove();
	void EndSpecialMove();

	//================================
	// アニメーション
	//================================
	void PlayAnimation(PlayerAnimationType type);

	// アニメーションが終わったかどうかを返す
	bool IsAnimationFinished()const { return m_animation.IsFinished(); }

	//================================
	// その他
	//================================

	void SetGroundYPos(const float pos) { m_groundYPos = pos; }
	float GetGroundYPos() const { return m_groundYPos; }

	std::weak_ptr<CameraBase>GetCamera()const { return m_wpCamera; }

private:

	// デバッグ用のコマンド(Tキーで回復)
	void UpdateDebugCommand();

	// 入力を受け付ける
	void UpdateInput();


	void UpdateAnimation();

	void UpdateGroundPosY();

	// 攻撃判定・トレイルのフレーム区間をセットする
	void ApplyActionTiming(const PlayerActionTiming& timing);

	//================================
	// カメラ基準の移動・向き
	//================================

	// カメラの前方向(水平)を取得する。カメラが無い場合はfalseを返す
	bool GetCameraForward(Math::Vector3& outDir) const;

	// 攻撃時のキャラの向き(カメラの前方向へ向ける)
	void FacingDirectionToCamera();

	// カメラ基準の方向へ、指定した速度で水平移動を加える
	// (通常移動・攻撃中の移動で共通して使う)
	void ApplyCameraRelativeMove(const float speed);

	//================================
	// 当たり判定
	//================================

	// 攻撃判定のスフィアを作る
	DirectX::BoundingSphere CreateAttackSphere()             const;

	// 必殺技判定のスフィアを作る
	DirectX::BoundingSphere CreateSpecialMoveSphere()        const;


private:

	// 入力
	PlayerInput       m_input;

	// 各アクションクラス
	PlayerAttack      m_playerAttack;
	PlayerSpecialMove m_playerSpecialMove;
	PlayerGuard       m_playerGuard;

	// 攻撃判定(当たった相手の記録もここで持つ)
	PlayerHitChecker  m_hitChecker;

	// 剣の軌跡
	PlayerSwordTrail  m_playerSwordTrail;

	float           m_groundYPos = 0.0f;

	// カメラ
	std::weak_ptr<CameraBase> m_wpCamera;

	// アニメーションクラス
	PlayerAnimation           m_animation;

	// パラメータークラス
	PlayerParameter           m_parameter;

	// ステートマシン
	StateMachine<Player>      m_stateMachine;

	const std::string                    m_flyTextPath = "DamageNumber_Red.png";
};
