#include "TimeManager.h"

#include"../../main.h"

void TimeManager::Init()
{

}

void TimeManager::Update()
{
	float rawDeltaTime = Application::Instance().GetDeltaTime();


	// rawDeltaTime と kMaxDeltaTime の
	// 小さい方をm_deltaTimeへ入れる
	m_unscaledDeltaTime = std::min(rawDeltaTime, kMaxDeltaTime);

	UpdateHitStop();



	m_deltaTime = m_unscaledDeltaTime*m_timeScale;


}

void TimeManager::StartHitStop(const float duration)
{
	m_hitStopTimer = duration;
	m_isHitStop = true;
	m_timeScale = 0.0f;
}

void TimeManager::UpdateHitStop()
{
	if (!m_isHitStop)
	{
		return;
	}

	m_hitStopTimer -= m_unscaledDeltaTime;

	if (m_hitStopTimer <= 0.0f)
	{
		m_hitStopTimer = 0.0f;
		m_isHitStop = false;
		m_timeScale = 1.0f;
	}
}
