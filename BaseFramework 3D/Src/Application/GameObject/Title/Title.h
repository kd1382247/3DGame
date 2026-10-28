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

	void DrawButton(const std::shared_ptr<KdTexture>tex, const Math::Vector2& pos, bool isSelected);

	void UpdateSinCurve();

	// 選べるボタン
	enum class ButtonType
	{
		Start,
		Setting,
		None
	};

	// ボタンの操作 (キー・マウス)
	void UpdateButtonInput();

	// ボタンを選ぶ / 決定する
	void Select(ButtonType button);
	void Decide(ButtonType button);

	// マウスがボタンの上にいるか
	bool IsHoverButton(const POINT& mousePos, const Math::Vector2& buttonPos)const;

	// タイトルロゴを光らせる
	void UpdateTitleLogoShine();

	// ボタンを光らせる
	void UpdateButtonShine();

	std::shared_ptr<KdTexture>m_spTitleBGTex       = nullptr;
	std::shared_ptr<KdTexture>m_spCharactersTex    = nullptr;
	std::shared_ptr<KdTexture>m_spTitleLogoTex     = nullptr;

	// ボタン
	std::shared_ptr<KdTexture>m_spStartButtonTex   = nullptr;
	std::shared_ptr<KdTexture>m_spSettingButtonTex = nullptr;


	// 今選ばれているボタン (キー・マウスで切り替わる。選ばれているボタンは大きく描く)
	ButtonType m_selectedButton = ButtonType::Start;

	// 設定画面 (シーン側が所有しているので、こちらはweak_ptrで持つ)
	std::weak_ptr<Setting> m_wpSetting;

	// 前フレームの入力 (「押した瞬間」「マウスが動いた」を取るため)
	bool  m_isPrevMouseDown = false;
	POINT m_prevMousePos    = {};

	const Math::Vector2 ButtonTexSize = { 410.0f,70.0f };

	Math::Vector2 m_startButtonPos   = {0.0f,-200.0f};
	Math::Vector2 m_settingButtonPos = {0.0f,-300.0f};

	float m_angle = 0.0f;

	float m_charactersTexPosY = 0.0f;

	// 光の帯
	float m_shinePos = -0.15f;      // 帯の位置 (UV.x)
	float   m_shineWaitCount = 0;   // 次の光までの待ちフレーム

	const float ShineWidth = 0.1f;  // 帯の幅
	const float ShineSpeed = 0.5f;  // 1秒で進む量
	const float ShineIntensity = 1.5f;   // 光の強さ 
	const float   ShineWaitTime = 2.0f; // 光り終わってからの待ち

	// ボタンの光
	const float ButtonShineWidth = 0.15f;      // 帯の幅
	const float ButtonShineSpeed = 0.8f;      // 1秒で進む量
	const float ButtonShineWaitTime = 2.0f;   // 光り終わってからの待ち(秒)
	const float ButtonShineIntensity = 2.5f;  // 光の強さ
	const float ButtonShineSlope = ButtonTexSize.y / ButtonTexSize.x;   // 傾き係数

	float m_buttonShinePos = -ButtonShineWidth; // 帯の位置
	float m_buttonShineWaitTime = 0.0f;         // 待ち残り時間(秒)




	const std::string FilePath = "Asset/Textures/UI/Title/";

};