#include "ResultUI.h"

#include"../../System/Mouse/Mouse.h"
#include"../../System/TimeManager/TimeManager.h"
#include"../../System/GameResult/GameResult.h"

namespace
{
	// 座標は「画面中央が(0,0)・上がプラス」 (Mouse::Get2DMousePos と同じ)

	// 演出のタイミング (秒。0は表示が始まった瞬間)
	constexpr float FrameStartTime  = 0.2f;	// 枠・文字・数字が出始める時間
	constexpr float FrameFadeTime   = 0.5f;	// 枠・文字・数字が出きるまでの時間
	constexpr float ButtonStartTime = 0.9f;	// ボタンが出始める時間
	constexpr float ButtonFadeTime  = 0.4f;	// ボタンが出きるまでの時間

	// 「Result」の文字 (画像の大きさ 340x90)。クリアタイムの枠の上に置く
	constexpr int ResultTextY = 215;	// 文字の中心

	// クリアタイムの枠 (画像の大きさ 520x224) と、その中の並び
	constexpr int FrameY    = 40;	// 枠の中心
	constexpr int TimeTextY = 92;	// 「ClearTime」の文字の中心 (画像の大きさ 380x80)
	constexpr int DigitY    = -10;	// 数字の中心 (画像の大きさ 高さ64)

	// 数字の画像 (0～9が横に10個並んでいる) の分け方
	constexpr int DigitCount = 10;

	// 表示できるクリアタイムの上限 (99分59秒)
	constexpr int MaxSeconds = 99 * 60 + 59;

	// ボタン (画像の大きさ 300x50。左: RETRY / 右: TITLE)
	// 当たり判定(クリック・マウスが乗る範囲)も、この大きさを使う
	constexpr int ButtonY       = -170;
	constexpr int ButtonOffsetX = 180;	// 画面の中央から左右へのずれ (ボタンの中心まで。2つの間に60pxあく)
	constexpr int ButtonWidth   = 300;
	constexpr int ButtonHeight  = 50;

	// 選ばれているボタンの大きさの倍率 (ゲームオーバーのボタンと同じく、少し大きくする)
	constexpr float ButtonSelectScale = 1.1f;

	// 選ばれていないボタンの明るさ
	constexpr float ButtonDimBrightness = 0.6f;

	// 効果音のファイル名 (素材ができたら入れる。空のままなら鳴らさない)
	const std::string MoveSE = "Asset/Data/Sound/SE/System/Select.wav";	// 選んでいる項目が変わった時
	const std::string DecideSE = "Asset/Data/Sound/SE/System/Confirm.wav";	// 決定した時

	// テクスチャの置き場
	const std::string TexFilePath = "Asset/Textures/Result/";

	std::shared_ptr<KdTexture> LoadTexture(const std::string& fileName)
	{
		std::shared_ptr<KdTexture> tex = std::make_shared<KdTexture>();
		tex->Load(TexFilePath + fileName);

		return tex;
	}

	// 効果音を鳴らす (ファイル名が空なら何もしない)
	void PlaySE(const std::string& fileName)
	{
		if (fileName.empty())
		{
			return;
		}

		KdAudioManager::Instance().PlaySE(fileName);
	}

	// キー(マウスのボタン)が押されているか
	bool IsKeyDown(int key)
	{
		return (GetAsyncKeyState(key) & 0x8000) != 0;
	}

	// 項目の、ボタンの中心のX座標
	int GetButtonX(ResultUI::Choice choice)
	{
		return (choice == ResultUI::Choice::Retry) ? -ButtonOffsetX : ButtonOffsetX;
	}
}

void ResultUI::Init()
{
	m_spBackTex        = LoadTexture("Result_bg.png");
	m_spResultTextTex  = LoadTexture("result_text.png");
	m_spFrameTex       = LoadTexture("ClearTime_frame.png");
	m_spTimeTextTex    = LoadTexture("cleartime_text.png");
	m_spDigitsTex      = LoadTexture("white_digits.png");
	m_spColonTex       = LoadTexture("white_colon.png");
	m_spRetryButtonTex = LoadTexture("RetryButton.png");
	m_spTitleButtonTex = LoadTexture("TitleButton.png");

	m_timer      = 0.0f;
	m_isMenuOpen = false;
	m_selected   = Choice::Retry;
	m_choice     = Choice::None;

	// ゲームシーンが記録したクリアタイムを読む
	m_clearTime = GameResult::Instance().GetClearTime();
}

void ResultUI::Update()
{
	m_timer += TimeManager::Instance().GetUnscaleeDeltaTime();

	if (!m_isMenuOpen)
	{
		// ボタンが出きったら、選べるようにする
		if (m_timer >= ButtonStartTime + ButtonFadeTime)
		{
			OpenMenu();
		}

		return;
	}

	UpdateMenu();
}

void ResultUI::DrawSprite()
{
	// 背景 (画面いっぱい。1280x720)
	DrawImage(m_spBackTex, 0, 0, 1.0f, 1.0f, 1.0f);

	// 「Result」の文字と、クリアタイムの枠・文字・数字 (だんだん濃くなる)
	const float frameRate = std::clamp((m_timer - FrameStartTime) / FrameFadeTime, 0.0f, 1.0f);

	if (frameRate > 0.0f)
	{
		DrawImage(m_spResultTextTex, 0, ResultTextY, 1.0f, frameRate, 1.0f);

		DrawImage(m_spFrameTex,    0, FrameY,    1.0f, frameRate, 1.0f);
		DrawImage(m_spTimeTextTex, 0, TimeTextY, 1.0f, frameRate, 1.0f);

		DrawClearTime(DigitY, frameRate);
	}

	// ボタン
	const float buttonRate = std::clamp((m_timer - ButtonStartTime) / ButtonFadeTime, 0.0f, 1.0f);

	if (buttonRate > 0.0f)
	{
		DrawButton(Choice::Retry, m_spRetryButtonTex, buttonRate);
		DrawButton(Choice::Title, m_spTitleButtonTex, buttonRate);
	}
}

void ResultUI::OpenMenu()
{
	m_isMenuOpen = true;

	// 開いた時に押されていたキーやボタンで、いきなり決定されないようにする
	m_isPrevLeftKey   = IsKeyDown('A') || IsKeyDown(VK_LEFT);
	m_isPrevRightKey  = IsKeyDown('D') || IsKeyDown(VK_RIGHT);
	m_isPrevEnterKey  = IsKeyDown(VK_RETURN);
	m_isPrevMouseDown = IsKeyDown(VK_LBUTTON);
	m_prevMousePos    = Mouse::Instance().Get2DMousePos();
}

void ResultUI::UpdateMenu()
{
	const bool  isLeftKey   = IsKeyDown('A') || IsKeyDown(VK_LEFT);
	const bool  isRightKey  = IsKeyDown('D') || IsKeyDown(VK_RIGHT);
	const bool  isEnterKey  = IsKeyDown(VK_RETURN);
	const bool  isMouseDown = IsKeyDown(VK_LBUTTON);
	const POINT mousePos    = Mouse::Instance().Get2DMousePos();

	// 決定した後は、入力を受け付けない
	if (m_choice == Choice::None)
	{
		// 左右のキーで選ぶ (押した瞬間だけ)
		if (isLeftKey && !m_isPrevLeftKey)
		{
			Select(Choice::Retry);
		}

		if (isRightKey && !m_isPrevRightKey)
		{
			Select(Choice::Title);
		}

		// マウスは、動いた時だけ、乗っているボタンを選ぶ
		// (止まっている間も選ぶと、キーで選んだ項目をすぐ上書きしてしまう)
		const Choice hover = GetHoverButton(mousePos);

		const bool isMouseMoved = mousePos.x != m_prevMousePos.x || mousePos.y != m_prevMousePos.y;

		if (isMouseMoved && hover != Choice::None)
		{
			Select(hover);
		}

		// 決定 (Enter か、ボタンの上でクリック)
		if (isEnterKey && !m_isPrevEnterKey)
		{
			Decide(m_selected);
		}
		else if (isMouseDown && !m_isPrevMouseDown && hover != Choice::None)
		{
			Decide(hover);
		}
	}

	m_isPrevLeftKey   = isLeftKey;
	m_isPrevRightKey  = isRightKey;
	m_isPrevEnterKey  = isEnterKey;
	m_isPrevMouseDown = isMouseDown;
	m_prevMousePos    = mousePos;
}

void ResultUI::Select(Choice choice)
{
	if (choice == m_selected)
	{
		return;
	}

	m_selected = choice;

	PlaySE(MoveSE);
}

void ResultUI::Decide(Choice choice)
{
	m_selected = choice;
	m_choice   = choice;

	PlaySE(DecideSE);
}

ResultUI::Choice ResultUI::GetHoverButton(const POINT& mousePos) const
{
	// ボタンの高さの範囲に入っていなければ、どのボタンにも乗っていない
	if (std::abs(mousePos.y - ButtonY) > ButtonHeight / 2)
	{
		return Choice::None;
	}

	if (std::abs(mousePos.x - GetButtonX(Choice::Retry)) <= ButtonWidth / 2)
	{
		return Choice::Retry;
	}

	if (std::abs(mousePos.x - GetButtonX(Choice::Title)) <= ButtonWidth / 2)
	{
		return Choice::Title;
	}

	return Choice::None;
}

void ResultUI::DrawImage(std::shared_ptr<KdTexture> tex, int x, int y, float scale, float alpha, float brightness)
{
	if (!tex)
	{
		return;
	}

	const int texW = static_cast<int>(tex->GetWidth());
	const int texH = static_cast<int>(tex->GetHeight());

	const Math::Rectangle srcRect = { 0, 0, texW, texH };
	const Math::Color     color   = { brightness, brightness, brightness, alpha };

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		tex, x, y,
		static_cast<int>(texW * scale), static_cast<int>(texH * scale),
		&srcRect, &color);
}

void ResultUI::DrawButton(Choice choice, std::shared_ptr<KdTexture> tex, float alpha)
{
	// 選ばれているボタンは、明るく少し大きく描く
	const bool isSelected = (choice == m_selected);

	const float scale      = isSelected ? ButtonSelectScale : 1.0f;
	const float brightness = isSelected ? 1.0f : ButtonDimBrightness;

	DrawImage(tex, GetButtonX(choice), ButtonY, scale, alpha, brightness);
}

void ResultUI::DrawClearTime(int y, float alpha)
{
	if (!m_spDigitsTex || !m_spColonTex)
	{
		return;
	}

	// 秒の単位の整数にする (小数点以下は切り捨て。表示できる上限を超えたら、上限で止める)
	const int totalSeconds = std::clamp(static_cast<int>(m_clearTime), 0, MaxSeconds);

	const int minutes = totalSeconds / 60;
	const int seconds = totalSeconds % 60;

	// 並べるもの : 分(2桁) : 秒(2桁)   (-1は「:」)
	const int items[] =
	{
		minutes / 10, minutes % 10, -1,
		seconds / 10, seconds % 10
	};

	// 数字の1つ分の幅 (画像を10等分する)
	const int digitW = static_cast<int>(m_spDigitsTex->GetWidth()) / DigitCount;
	const int colonW = static_cast<int>(m_spColonTex->GetWidth());

	// 全体の幅を求めて、画面の中央に並ぶように左端を決める
	int totalW = 0;

	for (const int item : items)
	{
		totalW += (item < 0) ? colonW : digitW;
	}

	int x = -totalW / 2;

	for (const int item : items)
	{
		if (item < 0)
		{
			DrawImage(m_spColonTex, x + colonW / 2, y, 1.0f, alpha, 1.0f);

			x += colonW;
		}
		else
		{
			DrawDigit(item, x + digitW / 2, y, alpha);

			x += digitW;
		}
	}
}

void ResultUI::DrawDigit(int digit, int x, int y, float alpha)
{
	const int digitW = static_cast<int>(m_spDigitsTex->GetWidth()) / DigitCount;
	const int digitH = static_cast<int>(m_spDigitsTex->GetHeight());

	// 画像の中から、その数字の部分だけを切り出して描く
	const Math::Rectangle srcRect = { digit * digitW, 0, digitW, digitH };
	const Math::Color     color   = { 1.0f, 1.0f, 1.0f, alpha };

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spDigitsTex, x, y,
		digitW, digitH,
		&srcRect, &color);
}
