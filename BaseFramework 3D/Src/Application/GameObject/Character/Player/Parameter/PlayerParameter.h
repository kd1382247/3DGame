#pragma once

struct Parameter
{
	int m_maxHP = 0;
	float m_attackPow = 0;
	float m_moveSpeed = 0;
};

class PlayerParameter
{
public:

	PlayerParameter(){}
	~PlayerParameter(){}

	void Init();



private:

	Parameter m_param = {};
};



