#include "SceneTransition.h"

void SceneTransition::Init()
{
	if (!m_spBlackFadeMask)
	{
		m_spBlackFadeMask = std::make_shared<KdTexture>();
		m_spBlackFadeMask->Load("Asset/Textures/BlackFadeMask/BlackFadeMask.png");
	}
}

void SceneTransition::StartFadeOut()
{
	if (m_state != State::Idle)
	{
		return;
	}

	m_state = State::FadeOut;
	m_alpha = 0.0f;
}

void SceneTransition::StartFadeIn()
{
	if (m_state != State::BlackOut)
	{
		return;
	}

	m_state = State::FadeIn;
	m_alpha = 1.0f;
}

void SceneTransition::Update()
{

	// Idleの時は何もしない
	if (m_state == State::Idle)
	{
		return;
	}


	if (m_state == State::FadeOut)
	{
		m_alpha += m_fadeSpeed;

		if (m_alpha >= 1.0f)
		{
			m_alpha = 1.0f;
			m_state = State::BlackOut;
		}
	}
	else if (m_state == State::FadeIn)
	{
		m_alpha -= m_fadeSpeed;

		if (m_alpha <= 0.0f)
		{
			m_alpha = 0.0f;
			m_state = State::Idle;
		}
	}
}

void SceneTransition::Draw()
{
	if (!m_spBlackFadeMask)
	{
		return;
	}

	if (m_state == State::Idle)
	{
		return;
	}

	Math::Color color = { 1,1,1,m_alpha };

	KdShaderManager::Instance().m_spriteShader.Begin();

	KdShaderManager::Instance().
		m_spriteShader.DrawTex(m_spBlackFadeMask, 0, 0,nullptr,&color);

	KdShaderManager::Instance().m_spriteShader.End();
}

