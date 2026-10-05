#pragma once

class Title:public KdGameObject
{

public:

	void Init()override;
	void Update()override;
	void DrawSprite()override;


private:

	void InitTexture(std::shared_ptr<KdTexture>& tex, std::string fileName);

	void DrawButton(const std::shared_ptr<KdTexture>tex, const Math::Vector2& pos, bool isHover);

	void UpdateSinCurve();

	void UpdateButtonInput();

	std::shared_ptr<KdTexture>m_spTitleBGTex       = nullptr;
	std::shared_ptr<KdTexture>m_spCharactersTex    = nullptr;
	std::shared_ptr<KdTexture>m_spTitleLogoTex     = nullptr;

	// ボタン
	std::shared_ptr<KdTexture>m_spStartButtonTex   = nullptr;
	std::shared_ptr<KdTexture>m_spSettingButtonTex = nullptr;


	// スタートボタン上にマウスがあるか
	bool m_isHoverStartButton = false;
	// セッティングボタン上にマウスがあるか
	bool m_isHoverSettingButton = false;

	const Math::Vector2 ButtonTexSize = { 410.0f,70.0f };

	Math::Vector2 m_startButtonPos   = {0.0f,-200.0f};
	Math::Vector2 m_settingButtonPos = {0.0f,-300.0f};

	float m_angle = 0.0f;

	float m_charactersTexPosY = 0.0f;

	const std::string FilePath = "Asset/Textures/Title/";

};