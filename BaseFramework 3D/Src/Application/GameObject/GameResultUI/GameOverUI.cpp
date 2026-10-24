#include "GameOverUI.h"

#include"../../System/Mouse/Mouse.h"
#include"../../System/TimeManager/TimeManager.h"

namespace
{
	// 座標は「画面中央が(0,0)・上がプラス」 (Mouse::Get2DMousePos と同じ)

	// 画面サイズの半分 (1280x720)
	constexpr int ScreenHalfW = 640;
	constexpr int ScreenHalfH = 360;

	// 演出のタイミング (秒。0は演出が始まった瞬間)
	constexpr float DimFadeTime     = 0.8f;	// 画面が暗くなるまでの時間
	constexpr float TextStartTime   = 0.6f;	// 「GAME OVER」が出始める時間
	constexpr float TextFadeTime    = 1.0f;	// 「GAME OVER」が出きるまでの時間
	constexpr float ButtonStartTime = 1.8f;	// ボタンが出始める時間
	constexpr float ButtonFadeTime  = 0.5f;	// ボタンが出きるまでの時間

	// 画面を暗くする濃さ (最大)
	constexpr float DimMaxAlpha = 0.65f;

	// 「GAME OVER」を置く位置 (画像の中心)
	constexpr int TextY = 100;

	// 「GAME OVER」が出てくる時の最初の大きさ (1.0に向かって縮む)
	constexpr float TextStartScale = 1.2f;

	// ボタン (画像の大きさ 300x50。左: RETRY / 右: TITLE)
	// 当たり判定(クリック・マウスが乗る範囲)も、この大きさを使う
	constexpr int ButtonY       = -80;
	constexpr int ButtonOffsetX = 180;	// 画面の中央から左右へのずれ (ボタンの中心まで。2つの間に60pxあく)
	constexpr int ButtonWidth   = 300;
	constexpr int ButtonHeight  = 50;

	// 選ばれているボタンの大きさの倍率 (タイトルのボタン・設定の閉じるボタンと同じく、少し大きくする)
	constexpr float ButtonSelectScale = 1.1f;

	// 選ばれていないボタンの明るさ
	constexpr float ButtonDimBrightness = 0.6f;

	// 効果音のファイル名 (素材ができたら入れる。空のままなら鳴らさない)
	const std::string MoveSE   = "Asset/Data/Sound/SE/System/Select.wav";	// 選んでいる項目が変わった時
	const std::string DecideSE = "Asset/Data/Sound/SE/System/Confirm.wav";	// 決定した時

	// テクスチャの置き場
	const std::string TexFilePath = "Asset/Textures/UI/Result/";

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
	int GetButtonX(GameOverUI::Choice choice)
	{
		return (choice == GameOverUI::Choice::Retry) ? -ButtonOffsetX : ButtonOffsetX;
	}
}

void GameOverUI::Init()
{
	m_spTextTex        = LoadTexture("GameOver.png");
	m_spRetryButtonTex = LoadTexture("RetryButton.png");
	m_spTitleButtonTex = LoadTexture("TitleButton.png");

	m_timer      = 0.0f;
	m_isMenuOpen = false;
	m_selected   = Choice::Retry;
	m_choice     = Choice::None;
}

void GameOverUI::Update()
{
	// ヒットストップやスローの影響を受けないように、実時間の経過を使う
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

void GameOverUI::DrawSprite()
{
	KdSpriteShader& sprite = KdShaderManager::Instance().m_spriteShader;

	// 画面を暗くする
	const float dimRate = std::clamp(m_timer / DimFadeTime, 0.0f, 1.0f);
	const Math::Color dimColor = { 0.0f, 0.0f, 0.0f, DimMaxAlpha * dimRate };

	sprite.DrawBox(0, 0, ScreenHalfW, ScreenHalfH, &dimColor);

	// 「GAME OVER」 (だんだん濃くなりながら、少しずつ縮んで1.0倍になる)
	const float textRate = std::clamp((m_timer - TextStartTime) / TextFadeTime, 0.0f, 1.0f);

	if (textRate > 0.0f)
	{
		const float scale = TextStartScale + (1.0f - TextStartScale) * textRate;

		DrawImage(m_spTextTex, 0, TextY, scale, textRate, 1.0f);
	}

	// ボタン
	const float buttonRate = std::clamp((m_timer - ButtonStartTime) / ButtonFadeTime, 0.0f, 1.0f);

	if (buttonRate > 0.0f)
	{
		DrawButton(Choice::Retry, m_spRetryButtonTex, buttonRate);
		DrawButton(Choice::Title, m_spTitleButtonTex, buttonRate);
	}
}

void GameOverUI::OpenMenu()
{
	m_isMenuOpen = true;

	// 開いた時に押されていたキーやボタンで、いきなり決定されないようにする
	m_isPrevLeftKey   = IsKeyDown('A') || IsKeyDown(VK_LEFT);
	m_isPrevRightKey  = IsKeyDown('D') || IsKeyDown(VK_RIGHT);
	m_isPrevEnterKey  = IsKeyDown(VK_RETURN);
	m_isPrevMouseDown = IsKeyDown(VK_LBUTTON);
	m_prevMousePos    = Mouse::Instance().Get2DMousePos();
}

void GameOverUI::UpdateMenu()
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

void GameOverUI::Select(Choice choice)
{
	if (choice == m_selected)
	{
		return;
	}

	m_selected = choice;

	PlaySE(MoveSE);
}

void GameOverUI::Decide(Choice choice)
{
	m_selected = choice;
	m_choice   = choice;

	PlaySE(DecideSE);
}

GameOverUI::Choice GameOverUI::GetHoverButton(const POINT& mousePos) const
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

void GameOverUI::DrawImage(std::shared_ptr<KdTexture> tex, int x, int y, float scale, float alpha, float brightness)
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

void GameOverUI::DrawButton(Choice choice, std::shared_ptr<KdTexture> tex, float alpha)
{
	// 選ばれているボタンは、明るく少し大きく描く
	const bool isSelected = (choice == m_selected);

	const float scale      = isSelected ? ButtonSelectScale : 1.0f;
	const float brightness = isSelected ? 1.0f : ButtonDimBrightness;

	DrawImage(tex, GetButtonX(choice), ButtonY, scale, alpha, brightness);
}
