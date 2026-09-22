#pragma once

class SlimeParameter
{
public:

	struct Parameter
	{
		int  m_maxHP = 100;
		float m_attackPower = 10.0f;
		float m_moveSpeed = 0.15f;
		float m_jumpPow = 0.4f;
		float m_turnSpeed = 12.0f;

		// このサイズの拡大率(ステージ側のScale値には関係なく、サイズごとの固定値として使う)
		float m_scale = 1.0f;
	};

	// スライムの大きさ(パラメータをこのサイズごとに切り替える)
	enum class SlimeSize
	{
		Large,
		Small
	};

	// サイズを指定してパラメータを取得
	Parameter GetParam(const SlimeSize size)const;

	void Init();

	void DrawInspecter();


private:

	void SaveToJson();
	void LoadFromJson();

	// 小さいスライム
	Parameter m_paramSmall = { 100, 10.0f, 0.15f, 0.4f, 12.0f, 1.0f };
	// 大きいスライム
	Parameter m_paramLarge = { 100, 10.0f, 0.15f, 0.4f, 12.0f, 1.5f };

};


