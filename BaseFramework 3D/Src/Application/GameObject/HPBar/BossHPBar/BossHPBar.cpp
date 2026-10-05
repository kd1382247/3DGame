#include "BossHPBar.h"

#include"../../Character/Boss/BossBase.h"

void BossHPBar::Init()
{
	// 各テクスチャを初期化
	InitTexture(m_spFrameTex, "Frame.png");
	InitTexture(m_spFrameBackGroundTex, "Frame_bg.png");
	InitTexture(m_spBarGreenTex, "Bar_green.png");
	InitTexture(m_spBarRedTex, "Bar_red.png");

	m_spNamePrateTex = std::make_shared<KdTexture>();
	m_spNamePrateTex->Load("BossName/NamePrate"+m_bossName);

}

void BossHPBar::Update()
{
	auto spBoss = m_wpBoss.lock();

	if (!spBoss)
	{
		return;
	}

	m_hpRate = spBoss->GetHealth().GetHPRate();

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

void BossHPBar::DrawSprite()
{

	auto spBoss = m_wpBoss.lock();
	if (!spBoss)
	{
		return;
	}

	DrawNamePrate();

	DrawFrameBackGround();
	DrawBarRed();
	DrawBarGreen();
	DrawFrame();
}

void BossHPBar::InitTexture(std::shared_ptr<KdTexture>& tex, const std::string& fileName)
{
	if (tex)
	{
		return;
	}

	tex = std::make_shared<KdTexture>();
	tex->Load(FilePath + fileName);
}

void BossHPBar::DrawFrame()
{
	DrawBar(m_spFrameTex);
}

void BossHPBar::DrawFrameBackGround()
{
	DrawBar(m_spFrameBackGroundTex);
}

void BossHPBar::DrawBarRed()
{
	DrawBar(m_spBarRedTex, m_damageRate);
}

void BossHPBar::DrawBarGreen()
{
	DrawBar(m_spBarGreenTex, m_hpRate);
}

void BossHPBar::DrawBar(const std::shared_ptr<KdTexture>& tex, const float rate)
{
	if (!tex)
	{
		return;
	}

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

void BossHPBar::DrawNamePrate()
{

	if (!m_spNamePrateTex)
	{
		return;
	}

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spNamePrateTex,
		static_cast<int>(m_namePlatePos.x),
		static_cast<int>(m_namePlatePos.y),
		static_cast<int>(m_namePrateWidth),
		static_cast<int>(m_namePrateHeight));

}
