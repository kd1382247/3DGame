#pragma once

class Player;

class PlayerHPBar :public KdGameObject
{
public:

	PlayerHPBar(){}
	~PlayerHPBar(){}

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	void SetPlayer(const std::shared_ptr<Player>&player) { m_wpPlayer = player; }

private:

	void DrawBackground();
	void DrawDamageBar();
	void DrawHPBar();

	void DrawBar(const std::shared_ptr<KdTexture>& tex, const float rate = 1.0f);


	std::weak_ptr<Player>m_wpPlayer;

	std::shared_ptr<KdTexture>m_spBackground=nullptr;
	std::shared_ptr<KdTexture>m_spDamageBar=nullptr;
	std::shared_ptr<KdTexture>m_spHPBar = nullptr;


	const float m_barWidth = 200.0f;
	const float m_barHeight = 40.0f;

	float m_hpRate = 1.0f;
	float m_damageRate = 1.0f;

	// Texture描画の基準点
	const Math::Vector2 pivot = { 0.0f,0.5f };

	// Barの位置
	Math::Vector2 m_barOffset = {-400.0f,-300.0f};

};