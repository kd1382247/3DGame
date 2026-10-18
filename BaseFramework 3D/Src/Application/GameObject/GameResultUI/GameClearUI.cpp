#include "GameClearUI.h"

#include"../../System/TimeManager/TimeManager.h"

namespace
{
	// 座標は「画面中央が(0,0)・上がプラス」 (Mouse::Get2DMousePos と同じ)

	// 画面サイズの半分 (1280x720)
	constexpr int ScreenHalfW = 640;
	constexpr int ScreenHalfH = 360;

	// 演出のタイミング (秒。0は演出が始まった瞬間)
	constexpr float DimFadeTime   = 0.8f;	// 画面が暗くなるまでの時間
	constexpr float TextStartTime = 0.3f;	// 「STAGE CLEAR」が出始める時間
	constexpr float TextFadeTime  = 1.0f;	// 「STAGE CLEAR」が出きるまでの時間

	// 画面を暗くする濃さ (最大。クリアは明るい雰囲気なのでゲームオーバーより薄い)
	constexpr float DimMaxAlpha = 0.35f;

	// 「STAGE CLEAR」を置く位置 (画像の中心)
	constexpr int TextY = 40;

	// 「STAGE CLEAR」が出てくる時の最初の大きさ (1.0に向かって縮む)
	constexpr float TextStartScale = 1.2f;

	// テクスチャの置き場
	const std::string TexFilePath = "Asset/Textures/Result/";
}

void GameClearUI::Init()
{
	m_spTextTex = std::make_shared<KdTexture>();
	m_spTextTex->Load(TexFilePath + "StageClear.png");

	m_timer = 0.0f;
}

void GameClearUI::Update()
{
	// ヒットストップやスローの影響を受けないように、実時間の経過を使う
	m_timer += TimeManager::Instance().GetUnscaleeDeltaTime();
}

void GameClearUI::DrawSprite()
{
	KdSpriteShader& sprite = KdShaderManager::Instance().m_spriteShader;

	// 画面を少し暗くする
	const float dimRate = std::clamp(m_timer / DimFadeTime, 0.0f, 1.0f);
	const Math::Color dimColor = { 0.0f, 0.0f, 0.0f, DimMaxAlpha * dimRate };

	sprite.DrawBox(0, 0, ScreenHalfW, ScreenHalfH, &dimColor);

	// 「STAGE CLEAR」 (だんだん濃くなりながら、少しずつ縮んで1.0倍になる)
	const float textRate = std::clamp((m_timer - TextStartTime) / TextFadeTime, 0.0f, 1.0f);

	if (textRate <= 0.0f || !m_spTextTex)
	{
		return;
	}

	const float scale = TextStartScale + (1.0f - TextStartScale) * textRate;

	const int texW = static_cast<int>(m_spTextTex->GetWidth());
	const int texH = static_cast<int>(m_spTextTex->GetHeight());

	const Math::Rectangle srcRect = { 0, 0, texW, texH };
	const Math::Color     color   = { 1.0f, 1.0f, 1.0f, textRate };

	sprite.DrawTex(
		m_spTextTex, 0, TextY,
		static_cast<int>(texW * scale), static_cast<int>(texH * scale),
		&srcRect, &color);
}
