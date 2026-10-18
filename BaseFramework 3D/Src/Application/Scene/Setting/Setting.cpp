#include "Setting.h"

#include "../../../Framework/Audio/KdAudio.h"
#include "../../System/Mouse/Mouse.h"
#include "../../System/TimeManager/TimeManager.h"

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

	// 左右キーを押し続けた時に、音量が変わる速さ (1秒あたり。0.5なら、端から端まで2秒)
	constexpr float VolumeKeySpeed = 0.5f;

	// 選ばれているスライダーの見た目
	constexpr int   SelectBoxX      = -10;	// 行の背景の中心X
	constexpr int   SelectBoxHalfW  = 310;	// 行の背景の半分の幅
	constexpr int   SelectBoxHalfH  = 32;	// 行の背景の半分の高さ
	constexpr float KnobSelectScale = 1.3f;	// つまみの大きさの倍率

	// 選ばれているスライダーの行の背景の色
	const Math::Color SelectBoxColor = { 1.0f, 1.0f, 1.0f, 0.15f };

	// キー(マウスのボタン)が押されているか
	bool IsKeyDown(int key)
	{
		return (GetAsyncKeyState(key) & 0x8000) != 0;
	}
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
	m_selectedItem    = MenuItem::BGM;
	m_keyAdjustSlider = SliderType::None;

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
	// 入力の状態
	// 閉じている間も毎フレーム更新しておく (開いた瞬間に「押した瞬間」を正しく判定するため)
	const bool  isMouseDown = IsKeyDown(VK_LBUTTON);
	const bool  isUpKey     = IsKeyDown('W') || IsKeyDown(VK_UP);
	const bool  isDownKey   = IsKeyDown('S') || IsKeyDown(VK_DOWN);
	const bool  isEnterKey  = IsKeyDown(VK_RETURN);
	const POINT mousePos    = Mouse::Instance().Get2DMousePos();

	const bool isMouseTrigger = isMouseDown && !m_isPrevMouseDown;
	const bool isUpTrigger    = isUpKey      && !m_isPrevUpKey;
	const bool isDownTrigger  = isDownKey    && !m_isPrevDownKey;
	const bool isEnterTrigger = isEnterKey   && !m_isPrevEnterKey;
	const bool isMouseMoved   = mousePos.x != m_prevMousePos.x || mousePos.y != m_prevMousePos.y;

	m_isPrevMouseDown = isMouseDown;
	m_isPrevUpKey     = isUpKey;
	m_isPrevDownKey   = isDownKey;
	m_isPrevEnterKey  = isEnterKey;
	m_prevMousePos    = mousePos;

	if (!m_isOpen)
	{
		return;
	}

	// 開いた直後のフレームは入力を受け付けない
	// (設定ボタンを押したクリックやEnterが、そのまま設定画面側の入力になるのを防ぐ)
	if (m_isJustOpened)
	{
		m_isJustOpened = false;
		return;
	}

	// 閉じるボタン
	m_isHoverClose = IsHoverCloseButton(mousePos);

	if (isMouseTrigger && m_isHoverClose)
	{
		Close();
		return;
	}

	// マウスが動いた時は、乗っている項目を選ぶ (ドラッグ中は選び直さない)
	if (isMouseMoved && m_dragSlider == SliderType::None)
	{
		if (m_isHoverClose)
		{
			Select(MenuItem::Close);
		}
		else if (IsHoverSlider(mousePos, BGMBarY))
		{
			Select(MenuItem::BGM);
		}
		else if (IsHoverSlider(mousePos, SEBarY))
		{
			Select(MenuItem::SE);
		}
	}

	// スライダーをつかむ (押した瞬間に、どのスライダーの上にいるかを調べる)
	if (isMouseTrigger)
	{
		if (IsHoverSlider(mousePos, BGMBarY))
		{
			m_dragSlider = SliderType::BGM;
			Select(MenuItem::BGM);
		}
		else if (IsHoverSlider(mousePos, SEBarY))
		{
			m_dragSlider = SliderType::SE;
			Select(MenuItem::SE);
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

	// キー操作 (マウスでつかんでいる間は受け付けない)
	if (m_dragSlider == SliderType::None)
	{
		UpdateKeyInput(isUpTrigger, isDownTrigger, isEnterTrigger);
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

	// 選ばれているスライダーの行の背景
	if (m_selectedItem == MenuItem::BGM)
	{
		sprite.DrawBox(SelectBoxX, BGMBarY, SelectBoxHalfW, SelectBoxHalfH, &SelectBoxColor);
	}
	else if (m_selectedItem == MenuItem::SE)
	{
		sprite.DrawBox(SelectBoxX, SEBarY, SelectBoxHalfW, SelectBoxHalfH, &SelectBoxColor);
	}

	// スライダー (上: BGM / 下: SE)
	sprite.DrawTex(m_spLabelBGMTex, LabelX, BGMBarY);
	sprite.DrawTex(m_spLabelSETex,  LabelX, SEBarY);

	DrawSlider(BGMBarY, GetSliderValue(SliderType::BGM), m_selectedItem == MenuItem::BGM);
	DrawSlider(SEBarY,  GetSliderValue(SliderType::SE),  m_selectedItem == MenuItem::SE);

	// 閉じるボタン
	DrawCloseButton();
}

void Setting::Open()
{
	m_isOpen          = true;
	m_isJustOpened    = true;
	m_isHoverClose    = false;
	m_dragSlider      = SliderType::None;
	m_selectedItem    = MenuItem::BGM;
	m_keyAdjustSlider = SliderType::None;
}

void Setting::Close()
{
	// キーで音量を変えている途中なら、閉じる前に保存する
	if (m_keyAdjustSlider != SliderType::None)
	{
		m_keyAdjustSlider = SliderType::None;

		SaveVolume();
	}

	m_isOpen       = false;
	m_isJustOpened = false;
	m_isHoverClose = false;
	m_dragSlider   = SliderType::None;
}

bool Setting::UpdateKeyInput(bool isUpTrigger, bool isDownTrigger, bool isEnterTrigger)
{
	// 上下で項目を選ぶ (画面の上から ×、BGM、SE の順。端から先は反対側に回る)
	constexpr int ItemCount = 3;

	int item = static_cast<int>(m_selectedItem);

	if (isUpTrigger)
	{
		item = (item + ItemCount - 1) % ItemCount;
	}

	if (isDownTrigger)
	{
		item = (item + 1) % ItemCount;
	}

	Select(static_cast<MenuItem>(item));

	// ×を選んでいる時は、Enterで閉じる
	if (m_selectedItem == MenuItem::Close)
	{
		if (isEnterTrigger)
		{
			Close();

			return true;
		}

		return false;
	}

	// 左右のキーで音量を変える (押している間、少しずつ変わる)
	const bool isLeftKey  = IsKeyDown('A') || IsKeyDown(VK_LEFT);
	const bool isRightKey = IsKeyDown('D') || IsKeyDown(VK_RIGHT);

	float direction = 0.0f;

	if (isRightKey)
	{
		direction += 1.0f;
	}

	if (isLeftKey)
	{
		direction -= 1.0f;
	}

	const SliderType slider = GetSelectedSlider();

	if (direction != 0.0f)
	{
		// ヒットストップなどの影響を受けないように、実時間の経過を使う
		const float deltaTime = TimeManager::Instance().GetUnscaleeDeltaTime();

		const float value = GetSliderValue(slider) + direction * VolumeKeySpeed * deltaTime;

		SetSliderValue(slider, std::clamp(value, 0.0f, 1.0f));

		m_keyAdjustSlider = slider;
	}
	else
	{
		// キーを離した
		FinishKeyAdjust();
	}

	return false;
}

void Setting::Select(MenuItem item)
{
	if (item == m_selectedItem)
	{
		return;
	}

	// 音量を変えている途中で別の項目に移ったら、そこで調節を終える
	FinishKeyAdjust();

	m_selectedItem = item;
}

void Setting::FinishKeyAdjust()
{
	if (m_keyAdjustSlider == SliderType::None)
	{
		return;
	}

	// SEなら、確認用にSEを鳴らす (音量を耳で確かめられる)
	if (m_keyAdjustSlider == SliderType::SE)
	{
		PlayPreviewSE();
	}

	m_keyAdjustSlider = SliderType::None;

	// キーを離した時に保存する (毎フレームは保存しない)
	SaveVolume();
}

Setting::SliderType Setting::GetSelectedSlider() const
{
	switch (m_selectedItem)
	{
	case MenuItem::BGM:
		return SliderType::BGM;
	case MenuItem::SE:
		return SliderType::SE;
	default:
		return SliderType::None;
	}
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

void Setting::DrawSlider(int barY, float value, bool isSelected)
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

	// つまみ (選ばれている時は、少し大きく描く)
	if (isSelected && m_spKnobTex)
	{
		const int knobW = static_cast<int>(m_spKnobTex->GetWidth()  * KnobSelectScale);
		const int knobH = static_cast<int>(m_spKnobTex->GetHeight() * KnobSelectScale);

		sprite.DrawTex(m_spKnobTex, knobX, barY, knobW, knobH);
	}
	else
	{
		sprite.DrawTex(m_spKnobTex, knobX, barY);
	}
}

void Setting::DrawCloseButton()
{
	KdSpriteShader& sprite = KdShaderManager::Instance().m_spriteShader;

	// マウスが乗っている時や、キーで選ばれている時は、少し大きく描く (タイトルのボタンと同じ)
	const bool isBig = m_isHoverClose || m_selectedItem == MenuItem::Close;

	const int size = isBig ? CloseButtonHoverSize : CloseButtonHalf * 2;

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
