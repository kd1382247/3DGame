#include "RoomIntroUI.h"

#include"../../System/TimeManager/TimeManager.h"

namespace
{
	// 座標は「画面中央が(0,0)・上がプラス」 (Mouse::Get2DMousePos と同じ)

	// 表示のタイミング (秒。0は表示が始まった瞬間)
	constexpr float FadeInTime  = 1.5f;	// 文字がゆっくり現れるまでの時間
	constexpr float HoldTime    = 1.5f;	// 現れきってから、消え始めるまでの時間
	constexpr float FadeOutTime = 0.8f;	// 文字が消えるまでの時間

	// 文字を置く位置 (画像の中心)
	constexpr int TextY = 150;

	// 文字の大きさの倍率 (画像の大きさは 593x80 / 479x80)
	constexpr float TextScale = 1.5f;

	// 文字が現れる時の最初の大きさ (TextScale に向かって、少しずつ縮む。1.0より大きいと、最初は大きい)
	constexpr float TextStartScale = 1.1f;

	// 画像の置き場とファイル名
	const std::string TexFilePath    = "Asset/Textures/Stage/Intro/";
	const std::string WaveTexFile    = "MissionWave.png";
	const std::string AllKillTexFile = "MissionKillAll.png";
}

void RoomIntroUI::Init()
{
	m_timer      = 0.0f;
	m_isPlaying  = false;
	m_isFinished = false;
}

void RoomIntroUI::Start(Mission mission)
{
	const std::string& fileName = (mission == Mission::Wave) ? WaveTexFile : AllKillTexFile;

	m_spTextTex = std::make_shared<KdTexture>();
	m_spTextTex->Load(TexFilePath + fileName);

	m_timer      = 0.0f;
	m_isPlaying  = true;
	m_isFinished = false;
}

void RoomIntroUI::Update()
{
	if (!m_isPlaying)
	{
		return;
	}

	// ヒットストップやスローの影響を受けないように、実時間の経過を使う
	m_timer += TimeManager::Instance().GetUnscaleeDeltaTime();

	// 消え終わったら、演出が終わったことにする
	if (m_timer >= FadeInTime + HoldTime + FadeOutTime)
	{
		m_isPlaying  = false;
		m_isFinished = true;
	}
}

void RoomIntroUI::DrawSprite()
{
	if (!m_isPlaying || !m_spTextTex)
	{
		return;
	}

	// 濃さ (ゆっくり濃くなる → そのまま → 薄くなって消える)
	float alpha = 1.0f;

	if (m_timer < FadeInTime)
	{
		alpha = m_timer / FadeInTime;
	}
	else if (m_timer >= FadeInTime + HoldTime)
	{
		alpha = 1.0f - (m_timer - FadeInTime - HoldTime) / FadeOutTime;
	}

	alpha = std::clamp(alpha, 0.0f, 1.0f);

	// 大きさ (現れる間に、少し縮みながら TextScale になる)
	const float appearRate = std::clamp(m_timer / FadeInTime, 0.0f, 1.0f);
	const float scale      = TextScale * (TextStartScale + (1.0f - TextStartScale) * appearRate);

	const int texW = static_cast<int>(m_spTextTex->GetWidth());
	const int texH = static_cast<int>(m_spTextTex->GetHeight());

	const Math::Rectangle srcRect = { 0, 0, texW, texH };
	const Math::Color     color   = { 1.0f, 1.0f, 1.0f, alpha };

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spTextTex, 0, TextY,
		static_cast<int>(texW * scale), static_cast<int>(texH * scale),
		&srcRect, &color);
}
