#include "Setting.h"

#include "../../../Framework/Audio/KdAudio.h"
#include "../../System/Mouse/Mouse.h"

namespace
{
	// 座標は「画面中央が(0,0)・上がプラス」 (Mouse::Get2DMousePos と同じ)

	// 画面サイズの半分 (1280x720)
	constexpr int ScreenHalfW = 640;
	constexpr int ScreenHalfH = 360;

	// パネル(設定画面の背景)のハーフサイズ
	constexpr int PanelHalfW = 380;
	constexpr int PanelHalfH = 220;

	// スライダーのバー
	constexpr int BarCenterX = 60;
	constexpr int BarHalfW   = 200;
	constexpr int BarHalfH   = 6;
	constexpr int BarHeight  = BarHalfH * 2;
	constexpr int BarLeft    = BarCenterX - BarHalfW;
	constexpr int BarWidth   = BarHalfW * 2;

	// 上: BGM / 下: SE
	constexpr int BGMBarY = 0;
	constexpr int SEBarY  = -100;

	// つまみの半径
	constexpr int KnobRadius = 16;

	// スライダーをつかめる範囲(バーより少し広くとる)
	constexpr int GrabHalfW = BarHalfW + KnobRadius;
	constexpr int GrabHalfH = 24;

	// 閉じるボタン(パネルの右上)
	constexpr int CloseButtonX     = PanelHalfW - 40;
	constexpr int CloseButtonY     = PanelHalfH - 40;
	constexpr int CloseButtonHalf  = 24;
	constexpr int CloseButtonHoverSize = 56;	// マウスが乗った時の大きさ

	// 音量の保存先 (Asset/Data/Volume/Volume.json)
	const std::filesystem::path VolumeFolder = "Asset/Data/Volume";
	const std::filesystem::path VolumeFile   = VolumeFolder / "Volume.json";

	// 画像を置く位置 (画像の中心)
	constexpr int HeadingY        = 160;	// 「SETTING」の見出し
	constexpr int HeadingDividerY = 108;	// 見出しの下の区切り線
	constexpr int LabelX          = -240;	// 「BGM」「SE」のラベル

	// 画面を暗くする色
	const Math::Color DimColor = { 0.0f, 0.0f, 0.0f, 0.6f };
}

void Setting::Init()
{
	// 音量の読み込みはアプリ起動後の1回だけ行う
	// (シーンを移動して Setting が作り直されても、調整中の値を上書きしないため)
	static bool isVolumeLoaded = false;

	if (!isVolumeLoaded)
	{
		LoadVolume();

		isVolumeLoaded = true;
	}

	m_isOpen          = false;
	m_isJustOpened    = false;
	m_isHoverClose    = false;
	m_dragSlider      = SliderType::None;

	// テクスチャの読み込み
	InitTexture(m_spPanelTex,          "SettingsPanel.png");
	InitTexture(m_spHeadingTex,        "HeadingSetting.png");
	InitTexture(m_spHeadingDividerTex, "HeadingDivider.png");
	InitTexture(m_spLabelBGMTex,       "LabelBGM.png");
	InitTexture(m_spLabelSETex,        "LabelSE.png");
	InitTexture(m_spBarBackTex,        "SliderBar_bg.png");
	InitTexture(m_spBarFillTex,        "SliderBar_fill.png");
	InitTexture(m_spKnobTex,           "SliderKnob.png");
	InitTexture(m_spCloseButtonTex,    "CloseButton.png");
}

void Setting::Update()
{
	// マウスの左ボタンの状態
	// 閉じている間も毎フレーム更新しておく (開いた瞬間に「押した瞬間」を正しく判定するため)
	const bool isMouseDown    = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
	const bool isMouseTrigger = isMouseDown && !m_isPrevMouseDown;
	m_isPrevMouseDown = isMouseDown;

	if (!m_isOpen)
	{
		return;
	}

	// 開いた直後のフレームは入力を受け付けない
	// (設定ボタンを押したクリックが、そのまま設定画面側のクリックになるのを防ぐ)
	if (m_isJustOpened)
	{
		m_isJustOpened = false;
		return;
	}

	POINT mousePos = Mouse::Instance().Get2DMousePos();

	// 閉じるボタン
	m_isHoverClose = IsHoverCloseButton(mousePos);

	if (isMouseTrigger && m_isHoverClose)
	{
		Close();
		return;
	}

	// スライダーをつかむ (押した瞬間に、どのスライダーの上にいるかを調べる)
	if (isMouseTrigger)
	{
		if (IsHoverSlider(mousePos, BGMBarY))
		{
			m_dragSlider = SliderType::BGM;
		}
		else if (IsHoverSlider(mousePos, SEBarY))
		{
			m_dragSlider = SliderType::SE;
		}
	}

	// ドラッグ中の処理
	if (m_dragSlider != SliderType::None)
	{
		if (isMouseDown)
		{
			// 押している間は、マウスのX座標に合わせて値を変える
			SetSliderValue(m_dragSlider, CalcValueFromMouseX(mousePos.x));
		}
		else
		{
			// 離した時、SEなら確認用にSEを鳴らす (音量を耳で確かめられる)
			if (m_dragSlider == SliderType::SE)
			{
				PlayPreviewSE();
			}

			m_dragSlider = SliderType::None;

			// スライダーを離した時に保存する (毎フレームは保存しない)
			SaveVolume();
		}
	}
}

void Setting::DrawSprite()
{
	if (!m_isOpen)
	{
		return;
	}

	KdSpriteShader& sprite = KdShaderManager::Instance().m_spriteShader;

	// 画面全体を暗くする
	sprite.DrawBox(0, 0, ScreenHalfW, ScreenHalfH, &DimColor);

	// パネル
	sprite.DrawTex(m_spPanelTex, 0, 0);

	// 見出しと区切り線
	sprite.DrawTex(m_spHeadingTex,        0, HeadingY);
	sprite.DrawTex(m_spHeadingDividerTex, 0, HeadingDividerY);

	// スライダー (上: BGM / 下: SE)
	sprite.DrawTex(m_spLabelBGMTex, LabelX, BGMBarY);
	sprite.DrawTex(m_spLabelSETex,  LabelX, SEBarY);

	DrawSlider(BGMBarY, GetSliderValue(SliderType::BGM));
	DrawSlider(SEBarY,  GetSliderValue(SliderType::SE));

	// 閉じるボタン
	DrawCloseButton();
}

void Setting::Open()
{
	m_isOpen       = true;
	m_isJustOpened = true;
	m_isHoverClose = false;
	m_dragSlider   = SliderType::None;
}

void Setting::Close()
{
	m_isOpen       = false;
	m_isJustOpened = false;
	m_isHoverClose = false;
	m_dragSlider   = SliderType::None;
}

bool Setting::LoadVolume()
{
	// ファイルが無い場合は、初期値のまま (初回起動など)
	if (!std::filesystem::exists(VolumeFile))
	{
		return false;
	}

	std::ifstream file(VolumeFile);

	if (!file.is_open())
	{
		return false;
	}

	nlohmann::json volumeJson;

	try
	{
		file >> volumeJson;
	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		KdDebugGUI::Instance().AddErrorLog("%s\n", e.what());

		return false;
	}

	// BGM / SE それぞれ、数値で入っている項目だけ反映する
	// (範囲外の値は Set〜Volume の中で 0.0〜1.0 に丸められる)
	if (volumeJson.contains("BGMVolume") && volumeJson["BGMVolume"].is_number())
	{
		KdAudioManager::Instance().SetBGMVolume(volumeJson["BGMVolume"].get<float>());
	}

	if (volumeJson.contains("SEVolume") && volumeJson["SEVolume"].is_number())
	{
		KdAudioManager::Instance().SetSEVolume(volumeJson["SEVolume"].get<float>());
	}

	return true;
}

bool Setting::SaveVolume()
{
	// フォルダーが無い場合は作成
	std::filesystem::create_directories(VolumeFolder);

	// 小数第2位に丸めて保存する (0.800000011920929 のような長い数字になるのを防ぐ)
	nlohmann::json volumeJson;
	volumeJson["BGMVolume"] = std::round(KdAudioManager::Instance().GetBGMVolume() * 100.0) / 100.0;
	volumeJson["SEVolume"]  = std::round(KdAudioManager::Instance().GetSEVolume()  * 100.0) / 100.0;

	std::ofstream file(VolumeFile);

	if (!file.is_open())
	{
		return false;
	}

	file << volumeJson.dump(4);

	return true;
}

void Setting::InitTexture(std::shared_ptr<KdTexture>& tex, const std::string& fileName)
{
	if (tex)
	{
		return;
	}

	tex = std::make_shared<KdTexture>();
	tex->Load(TexFilePath + fileName);
}

float Setting::GetSliderValue(SliderType type) const
{
	switch (type)
	{
	case SliderType::BGM:
		return KdAudioManager::Instance().GetBGMVolume();
	case SliderType::SE:
		return KdAudioManager::Instance().GetSEVolume();
	default:
		return 0.0f;
	}
}

void Setting::SetSliderValue(SliderType type, float value)
{
	switch (type)
	{
	case SliderType::BGM:
		KdAudioManager::Instance().SetBGMVolume(value);
		break;
	case SliderType::SE:
		KdAudioManager::Instance().SetSEVolume(value);
		break;
	default:
		break;
	}
}

bool Setting::IsHoverSlider(const POINT& mousePos, int barY) const
{
	return  std::abs(mousePos.x - BarCenterX) <= GrabHalfW &&
			std::abs(mousePos.y - barY)       <= GrabHalfH;
}

bool Setting::IsHoverCloseButton(const POINT& mousePos) const
{
	return  std::abs(mousePos.x - CloseButtonX) <= CloseButtonHalf &&
			std::abs(mousePos.y - CloseButtonY) <= CloseButtonHalf;
}

float Setting::CalcValueFromMouseX(int mouseX) const
{
	// バーの左端が0.0、右端が1.0
	const float value = static_cast<float>(mouseX - BarLeft) / static_cast<float>(BarWidth);

	return std::clamp(value, 0.0f, 1.0f);
}

void Setting::DrawSlider(int barY, float value)
{
	KdSpriteShader& sprite = KdShaderManager::Instance().m_spriteShader;

	// 色をつける長さ(バーの左端〜つまみ)と、つまみの位置
	const int fillWidth = static_cast<int>(value * BarWidth);
	const int knobX     = BarLeft + fillWidth;

	// バーの背景
	sprite.DrawTex(m_spBarBackTex, BarCenterX, barY);

	// 値の分だけ、色つきのバーを表示する
	// (画像を左端から fillWidth 分だけ切り抜き、基準点を左端にしてバーの左端に合わせる)
	if (fillWidth > 0)
	{
		const Math::Rectangle srcRect = { 0, 0, fillWidth, BarHeight };

		sprite.DrawTex(m_spBarFillTex, BarLeft, barY, fillWidth, BarHeight, &srcRect, &kWhiteColor, { 0.0f, 0.5f });
	}

	// つまみ
	sprite.DrawTex(m_spKnobTex, knobX, barY);
}

void Setting::DrawCloseButton()
{
	KdSpriteShader& sprite = KdShaderManager::Instance().m_spriteShader;

	// マウスが乗っている時は、少し大きく描く (タイトルのボタンと同じ)
	const int size = m_isHoverClose ? CloseButtonHoverSize : CloseButtonHalf * 2;

	sprite.DrawTex(m_spCloseButtonTex, CloseButtonX, CloseButtonY, size, size);
}

void Setting::PlayPreviewSE()
{
	if (m_previewSEName.empty())
	{
		return;
	}

	KdAudioManager::Instance().PlaySE(m_previewSEName);
}
