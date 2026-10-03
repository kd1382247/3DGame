#pragma once

class PlayerParameter
{
public:

	// パラメータは「誰が使うか」ごとに構造体を分けている
	// 各アクションクラスには、自分の分だけをconst参照で渡す(Playerが中継するゲッターは持たない)
	//
	// 値の単位は Inspector に表示される。m=メートル s=秒 F=フレーム(60F=1秒) °=度 m/F=1フレームの移動量

	// コンボの最大数
	static constexpr int kComboCount = 3;

	// 体そのもののパラメータ(CharacterBaseが要求するもの・重力・地面との当たり)
	struct BodyParam
	{
		int   m_maxHP = 100;                        // [HP]  最大HP
		float m_turnSpeed = 12.0f;                  // [°/F] 回転速度(1フレームに回転できる最大角度)
		float m_gravityAcceleration = 72.0f;        // [m/s^2] 重力加速度

		float m_bumpPushRate = 0.1f;                // 押し戻しの影響を受ける割合(0〜1。小さいほど押されにくい)
		float m_maxWalkableSlopeAngle = 45.0f;      // [°]  登れる坂の最大角度

		float m_stepHeight = 0.2f;                  // [m]  越えられる段差の高さ(地面判定のレイを出す高さ)
		float m_groundRayLength = 100.0f;           // [m]  地面を探すレイの長さ
	};

	// 通常移動
	struct MoveParam
	{
		float m_moveSpeed = 9.0f;                   // [m/F] 移動速度(1フレームの移動量。実際の速さは 値 x 60 m/秒)
	};

	// ジャンプ
	struct JumpParam
	{
		float m_jumpPow = 0.4f;                     // ジャンプ力(飛び出しの勢い)
	};


	struct HitParam
	{
		float m_attackPower      = 10.0f;           // 攻撃力
		float m_knockBackPower   = 0.1f;            // ノックバックの強さ
		float m_hitStop          = 0.05f;           // [s] ヒットストップ
		float m_killHitStop      = 0.12f;           // [s] 撃破時のヒットストップ
		float m_SlowScale    = 0.5f;                // スロー演出の倍率
		float m_SlowDuration = 0.5f;                // スロー演出の時間
	};
	
	// 通常攻撃
	struct AttackParam
	{
		HitParam m_hitParam[kComboCount];

		float m_attackMoveSpeed  = 0.08f;           // [m/F] 攻撃中の移動速度
		float m_hitRadius        = 0.7f;            // [m]  攻撃判定の球の半径
		float m_hitForwardOffset = 0.8f;            // [m]  攻撃判定の球を、正面へどれだけ前に出すか
		
	};

	// ガード
	struct GuardParam
	{
		int   m_guardBreakCount      = 5;           // ガード中に何回攻撃を受けたら解除かの上限
		float m_guardHitResetTime    = 2.0f;        // [s] 一定時間攻撃を受けないときのリセット時間
		float m_guardKnockBackRate = 0.3f;          // ガード時に受けるノックバックの割合
  
	};

	// パリィ
	struct ParryParam
	{
		float m_parryWindow		     = 0.3f;		// [s] パリィの受付時間
		float m_parryKnockBackRadius = 1.5f;        // [m] パリィ成功時のノックバックの範囲 
		float m_parryHitStop		 = 0.1f;		// [s] パリィ成功時のヒットストップ
		float m_parrySlowScale       = 0.3;			// パリィ成功時のスロー倍率
		float m_parrySlowDuration    = 0.5f;        // パリィ成功時のスロー時間
		float m_parryKnockBackPower  = 0.3f;        // ノックバックの威力
		
	};

	// 必殺技
	struct SpecialMoveParam
	{
		HitParam m_hitParam =
		{ {20},{0.1f},{0.08f} };

		float m_moveSpeed = 0.3f;                   // [m/F] 必殺技中の移動速度

		// 多段ヒットの間隔
		float m_hitCooldownDuration = 5.0f;         // [F]  (フレーム数。60fps換算)

		float m_hitRadius = 1.5f;                   // [m]  攻撃判定の球の半径(自分を中心にした球)
	};

	const BodyParam&        GetBody()        const { return m_body; }
	const MoveParam&        GetMove()        const { return m_move; }
	const JumpParam&        GetJump()        const { return m_jump; }
	const AttackParam&      GetAttack()      const { return m_attack; }
	const SpecialMoveParam& GetSpecialMove() const { return m_specialMove; }
	const GuardParam&       GetGuard()       const { return m_guard; }
	const ParryParam&       GetParry()       const { return m_parry; }
	void Init();

	void DrawInspecter();


private:

	void SaveToJson();
	void LoadFromJson();

	BodyParam        m_body        = {};
	MoveParam        m_move        = {};
	JumpParam        m_jump        = {};
	AttackParam      m_attack      = {};
	SpecialMoveParam m_specialMove = {};
	GuardParam       m_guard       = {};
	ParryParam       m_parry       = {};
};
