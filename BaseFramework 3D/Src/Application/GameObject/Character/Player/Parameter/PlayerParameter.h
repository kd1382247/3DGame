#pragma once

class PlayerParameter
{
public:

	// パラメータは「誰が使うか」ごとに構造体を分けている
	// 各アクションクラスには、自分の分だけをconst参照で渡す(Playerが中継するゲッターは持たない)
	//
	// 値の単位は Inspector に表示される。m=メートル s=秒 F=フレーム(60F=1秒) °=度 m/F=1フレームの移動量

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

	// 通常攻撃
	struct AttackParam
	{
		float m_attackPower = 10.0f;                // 攻撃力
		float m_attackMoveSpeed = 0.08f;            // [m/F] 攻撃中の移動速度

		float m_hitRadius = 0.7f;                   // [m]  攻撃判定の球の半径
		float m_hitForwardOffset = 0.8f;            // [m]  攻撃判定の球を、正面へどれだけ前に出すか
		float m_knockBackPower = 0.1f;              // ノックバックの強さ
	};

	// 必殺技
	struct SpecialMoveParam
	{
		float m_attackPower = 20.0f;                // 攻撃力
		float m_moveSpeed = 0.3f;                   // [m/F] 必殺技中の移動速度

		// 多段ヒットの間隔
		float m_hitCooldownDuration = 5.0f;         // [F]  (フレーム数。60fps換算)

		float m_hitRadius = 1.5f;                   // [m]  攻撃判定の球の半径(自分を中心にした球)
		float m_knockBackPower = 0.1f;              // ノックバックの強さ
	};

	const BodyParam&        GetBody()        const { return m_body; }
	const MoveParam&        GetMove()        const { return m_move; }
	const JumpParam&        GetJump()        const { return m_jump; }
	const AttackParam&      GetAttack()      const { return m_attack; }
	const SpecialMoveParam& GetSpecialMove() const { return m_specialMove; }

	void Init();

	void DrawInspecter();


private:

	void SaveToJson();
	void LoadFromJson();

	BodyParam        m_body = {};
	MoveParam        m_move = {};
	JumpParam        m_jump = {};
	AttackParam      m_attack = {};
	SpecialMoveParam m_specialMove = {};
};
