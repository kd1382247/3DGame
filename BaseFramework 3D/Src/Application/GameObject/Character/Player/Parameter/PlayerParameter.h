#pragma once

class PlayerParameter
{
public:

	// パラメータは「誰が使うか」ごとに構造体を分けている
	// 各アクションクラスには、自分の分だけをconst参照で渡す(Playerが中継するゲッターは持たない)

	// 体そのもののパラメータ(CharacterBaseが要求するもの・重力)
	struct BodyParam
	{
		int   m_maxHP = 100;
		float m_turnSpeed = 12.0f;
		float m_gravityAcceleration = 72.0f;
	};

	// 通常移動
	struct MoveParam
	{
		float m_moveSpeed = 9.0f;
	};

	// ジャンプ
	struct JumpParam
	{
		float m_jumpPow = 0.4f;
	};

	// 通常攻撃
	struct AttackParam
	{
		float m_attackPower = 10.0f;
		float m_attackMoveSpeed = 0.08f;
	};

	// 必殺技
	struct SpecialMoveParam
	{
		float m_attackPower = 20.0f;
		float m_moveSpeed = 0.3f;

		// 多段ヒットの間隔(フレーム数。60fps換算)
		float m_hitCooldownDuration = 5.0f;
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
