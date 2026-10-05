#pragma once

class BossBase;

class BossHPBar :public KdGameObject
{
public:

	BossHPBar(){}
	~BossHPBar(){}

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	void SetBoss(const std::shared_ptr<BossBase>&boss){m_wpBoss=boss;}

	void SetBossName(const std::string& name,const float width,const float height) 
	{ 
		m_bossName = name+".png";
		m_namePrateWidth = width;
		m_namePrateHeight = height;
	}

private:

	void InitTexture(std::shared_ptr<KdTexture>& tex, const std::string& fileName);

	void DrawFrame();
	void DrawFrameBackGround();
	void DrawBarRed();
	void DrawBarGreen();

	void DrawBar(const std::shared_ptr<KdTexture>& tex, const float rate = 1.0f);

	void DrawNamePrate();

	std::weak_ptr<BossBase>m_wpBoss;

	// HPバー関連のテクスチャ
	std::shared_ptr<KdTexture>m_spFrameTex = nullptr;
	std::shared_ptr<KdTexture>m_spFrameBackGroundTex = nullptr;
	std::shared_ptr<KdTexture>m_spBarRedTex = nullptr;
	std::shared_ptr<KdTexture>m_spBarGreenTex = nullptr;

	// ネームプレート
	std::shared_ptr<KdTexture>m_spNamePrateTex = nullptr;

	const float m_barWidth = 700.0f;
	const float m_barHeight = 30.0f;

	float m_hpRate = 1.0f;
	float m_damageRate = 1.0f;

	// Texture描画の基準点
	const Math::Vector2 pivot = { 0.0f,0.5f };

	// Barの位置
	Math::Vector2 m_barOffset = { -400.0f,300.0f };

	const std::string FilePath = "Asset/Textures/HP/Boss/";

	std::string m_bossName = {};

	Math::Vector2 m_namePlatePos = { 0.0f,330.0f };
	float m_namePrateWidth = 0.0f;
	float m_namePrateHeight = 0.0f;

};