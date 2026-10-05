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

	void InitTexture(std::shared_ptr<KdTexture>& tex, const std::string& fileName);

	void DrawFrame();
	void DrawFrameBackGround();
	void DrawBarRed();
	void DrawBarGreen();

	void DrawBar(const std::shared_ptr<KdTexture>& tex, const float rate = 1.0f);


	std::weak_ptr<Player>m_wpPlayer;

	std::shared_ptr<KdTexture>m_spFrameTex = nullptr;
	std::shared_ptr<KdTexture>m_spFrameBackGroundTex=nullptr;
	std::shared_ptr<KdTexture>m_spBarRedTex=nullptr;
	std::shared_ptr<KdTexture>m_spBarGreenTex = nullptr;


	const float m_barWidth = 250.0f;
	const float m_barHeight = 50.0f;

	float m_hpRate = 1.0f;
	float m_damageRate = 1.0f;

	// Texture描画の基準点
	const Math::Vector2 pivot = { 0.0f,0.5f };

	// Barの位置
	Math::Vector2 m_barOffset = {-400.0f,-300.0f};

	const std::string FilePath = "Asset/Textures/HP/Player/";

};