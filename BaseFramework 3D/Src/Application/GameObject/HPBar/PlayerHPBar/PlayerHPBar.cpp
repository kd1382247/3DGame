#include "PlayerHPBar.h"

#include"../../Character/Player/Player.h"

void PlayerHPBar::Init()
{

	// 各テクスチャを初期化
	InitTexture(m_spFrameTex, "Frame.png");
	InitTexture(m_spFrameBackGroundTex, "Frame_bg.png");
	InitTexture(m_spBarGreenTex, "Bar_green.png");
	InitTexture(m_spBarRedTex, "Bar_red.png");

}

void PlayerHPBar::Update()
{
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	float maxHP = spPlayer->GetMaxHP();

	if (maxHP <= 0)
	{
		return;
	}

	float currentHP = spPlayer->GetCurrentHP();

	m_hpRate = std::clamp(currentHP / maxHP, 0.0f, 1.0f);

	// ダメージバーを減少
	if (m_damageRate > m_hpRate)
	{
		m_damageRate -= 0.004f;

		if (m_damageRate < m_hpRate)
		{
			m_damageRate = m_hpRate;
		}
	}
	else
	{
		m_damageRate = m_hpRate;
	}
}

void PlayerHPBar::DrawSprite()
{

	auto spPlayer = m_wpPlayer.lock();
	if (!spPlayer)
	{
		return;
	}

	
	DrawFrameBackGround();
	DrawBarRed();
	DrawBarGreen();
	DrawFrame();
}

void PlayerHPBar::InitTexture(std::shared_ptr<KdTexture>& tex, const std::string& filePath)
{
	if (tex)
	{
		return;
	}

	tex = std::make_shared<KdTexture>();
	tex->Load(MaterialPath + filePath);
}

void PlayerHPBar::DrawFrame()
{
	DrawBar(m_spFrameTex);
}

void PlayerHPBar::DrawFrameBackGround()
{
	DrawBar(m_spFrameBackGroundTex);
}

void PlayerHPBar::DrawBarRed()
{
	DrawBar(m_spBarRedTex,m_damageRate);
}

void PlayerHPBar::DrawBarGreen()
{
	DrawBar(m_spBarGreenTex, m_hpRate);
}

void PlayerHPBar::DrawBar(const std::shared_ptr<KdTexture>&tex, const float rate)
{
	const long w = static_cast<long>(m_barWidth * rate);
	const long h = static_cast<long>(m_barHeight);

	Math::Rectangle rc = { 0,0,w,h };
	Math::Color color = { 1.0f,1.0f,1.0f };

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		tex,
		static_cast<int>(m_barOffset.x),
		static_cast<int>(m_barOffset.y),
		static_cast<int>(w),
		static_cast<int>(h),
		&rc, &color, pivot);
}
