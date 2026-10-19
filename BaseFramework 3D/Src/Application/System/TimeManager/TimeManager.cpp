#include "TimeManager.h"

#include"../../main.h"

void TimeManager::Init()
{
	m_isPaused     = false;
	m_isHitStop    = false;
	m_hitStopTimer = 0.0f;
	m_isSlow       = false;
	m_maxSlowTimer = 0.0f;
	m_slowScale    = 1.0f;
	m_slowTimer    = 0.0f;
}

void TimeManager::Update()
{
	float rawDeltaTime = Application::Instance().GetDeltaTime();


	// rawDeltaTime と kMaxDeltaTime の
	// 小さい方をm_deltaTimeへ入れる
	m_unscaledDeltaTime = std::min(rawDeltaTime, kMaxDeltaTime);

	UpdateHitStop();

	UpdateSlowMotion();

	UpdateTimeScale();

	m_deltaTime = m_unscaledDeltaTime*m_timeScale;

}


void TimeManager::UpdateTimeScale()
{
	// 一時停止中か、ヒットストップ中の場合
	if (m_isPaused || IsHitStop())
	{
		m_timeScale = 0.0f;
	}
	else if (m_isSlow)
	{

		float rate = m_slowTimer / m_maxSlowTimer;
		rate = std::clamp(rate, 0.0f, 1.0f);

		m_timeScale = std::lerp(1.0f, m_slowScale, rate);
	}
	else
	{
		// どちらでもない場合
		m_timeScale = 1.0f;
	}
}

void TimeManager::StartHitStop(const float duration)
{
	// ヒットストップの時間が0以下の場合早期リターン
	if (duration <= 0)
	{
		return;
	}

	m_hitStopTimer = std::max(m_hitStopTimer,duration);
	m_isHitStop = true;
}

void TimeManager::UpdateHitStop()
{
	// 一時停止中は、残り時間を減らさない
	if (!m_isHitStop || m_isPaused)
	{
		return;
	}

	m_hitStopTimer -= m_unscaledDeltaTime;

	if (m_hitStopTimer <= 0.0f)
	{
		m_hitStopTimer = 0.0f;
		m_isHitStop = false;
	}
}

void TimeManager::StartSlowMotion(const float scale, const float duration)
{
	// スロー中は飛ばす
	if (m_isSlow)
	{
		return;
	}

	// スローの時間が0以下の場合早期リターン
	if (duration <= 0)
	{
		return;
	}

	float clScale = std::clamp(scale, 0.05f, 1.0f);

	m_slowScale = clScale;

	m_slowTimer = duration;
	m_maxSlowTimer = duration;

	m_isSlow = true;
}

void TimeManager::UpdateSlowMotion()
{

	// 一時停止中は、残り時間を減らさない
	if (!m_isSlow || m_isPaused)
	{
		return;
	}

	// ヒットストップ中は処理をしない
	if (IsHitStop())
	{
		return;
	}

	m_slowTimer -= m_unscaledDeltaTime;
	if (m_slowTimer <= 0)
	{
		m_slowScale = 1.0f;
		m_slowTimer = 0.0f;
		m_isSlow = false;
	}

}
