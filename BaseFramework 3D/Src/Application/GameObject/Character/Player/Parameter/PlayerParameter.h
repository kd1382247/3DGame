#pragma once

class PlayerParameter
{
public:

	struct Parameter
	{
		int  m_maxHP = 100;

		// Move
		float m_moveSpeed = 0.15f;
		float m_jumpPow = 0.4f;
		float m_turnSpeed = 12.0f;

		// Attack
		float m_attackPower = 10.0f;
		float m_attackMoveSpeed = 0.08f;

		// SpecialMove
		float m_specialAttackPower = 20.0f;
		float m_specialMoveSpeed = 0.3f;

		// その他
		float m_hitCooldownDuration = 5.0f;
	};

	 Parameter GetParam()const { return m_param; }

	void Init();

	void DrawInspecter();


private:

	void SaveToJson();
	void LoadFromJson();

	Parameter m_param = {};
};



