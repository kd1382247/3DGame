#pragma once


class MageParameter
{
public:

	struct Parameter
	{
		int  m_maxHP = 100;
		float m_attackPow = 10.0f;
		float m_moveSpeed = 0.15f;
		float m_jumpPow = 0.4f;
		float m_turnSpeed = 12.0f;
	};

	const Parameter& GetParam() { return m_param; }

	void Init();

	void DrawInspecter();


private:

	void SaveToJson();
	void LoadFromJson();

	Parameter m_param = {};
};



