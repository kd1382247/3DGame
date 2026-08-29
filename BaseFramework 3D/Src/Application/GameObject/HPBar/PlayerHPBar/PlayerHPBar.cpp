#include "PlayerHPBar.h"

#include"../../Character/Player/Player.h"

void PlayerHPBar::Init()
{
	if (!m_spBackground)
	{
		m_spBackground = std::make_shared<KdTexture>();
		m_spBackground->Load("Asset/Textures/HP/Background.png");
	}

	if (!m_spDamageBar)
	{
		m_spDamageBar = std::make_shared<KdTexture>();
		m_spDamageBar->Load("Asset/Textures/HP/DamageBar.png");
	}

	if (!m_spHPBar)
	{
		m_spHPBar = std::make_shared<KdTexture>();
		m_spHPBar->Load("Asset/Textures/HP/HPBar.png");
	}
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

}

void PlayerHPBar::DrawSprite()
{

	auto spPlayer = m_wpPlayer.lock();
	if (!spPlayer)
	{
		return;
	}

	DrawBackground();
	DrawDamageBar();
	DrawHPBar();

}

void PlayerHPBar::DrawBackground()
{
	DrawBar(m_spBackground);
}

void PlayerHPBar::DrawDamageBar()
{
	DrawBar(m_spDamageBar,m_damageRate);
}

void PlayerHPBar::DrawHPBar()
{
	DrawBar(m_spHPBar, m_hpRate);
}

void PlayerHPBar::DrawBar(const std::shared_ptr<KdTexture>&tex, const float rate)
{
	Math::Rectangle rc = { 0,0,static_cast<long>(m_barWidth),static_cast<long>(m_barHeight) };
	Math::Color color = { 1.0f,1.0f,1.0f };

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		tex, 
		static_cast<int>(m_barOffset.x),
		static_cast<int>(m_barOffset.y),
		static_cast<int>(m_barWidth * rate),
		static_cast<int>(m_barHeight),
		&rc, &color, pivot);
}
