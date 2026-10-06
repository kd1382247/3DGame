#pragma once

class Setting;

class Title:public KdGameObject
{

public:

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	// 設定画面をセットする (Settingボタンを押した時に開く)
	void SetSetting(const std::shared_ptr<Setting>& setting) { m_wpSetting = setting; }


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

	// 設定画面 (シーン側が所有しているので、こちらはweak_ptrで持つ)
	std::weak_ptr<Setting> m_wpSetting;

	// 前フレームでマウスの左ボタンが押されていたか (「押した瞬間」を取るため)
	bool m_isPrevMouseDown = false;

	const Math::Vector2 ButtonTexSize = { 410.0f,70.0f };

	Math::Vector2 m_startButtonPos   = {0.0f,-200.0f};
	Math::Vector2 m_settingButtonPos = {0.0f,-300.0f};

	float m_angle = 0.0f;

	float m_charactersTexPosY = 0.0f;

	const std::string FilePath = "Asset/Textures/Title/";

};