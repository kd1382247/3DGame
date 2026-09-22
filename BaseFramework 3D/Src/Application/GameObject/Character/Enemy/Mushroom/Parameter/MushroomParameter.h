#pragma once


class MushroomParameter
{
public:

	struct Parameter
	{
		int  m_maxHP = 100;
		float m_attackPower = 10.0f;
		float m_moveSpeed = 0.15f;
		float m_jumpPow = 0.4f;
		float m_turnSpeed = 12.0f;
	};

	// Mushroomのタイプ(パラメータをこのタイプごとに切り替える)
	enum class MushroomType
	{
		Smile,
		Angry
	};

	// タイプを指定してパラメータを取得
	Parameter GetParam(const MushroomType type)const;

	void Init();

	void DrawInspecter();


private:

	void SaveToJson();
	void LoadFromJson();

	Parameter m_paramSmile = {};
	Parameter m_paramAngry = {};
};


