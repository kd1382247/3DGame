#include "PlayerParry.h"

void PlayerParry::StartParry()
{
	m_parryTimer = 0.0f;
	m_isParryActive = true;
	m_isParrySuccess = false;
}

void PlayerParry::UpdateParryWindow(const float deltaTime)
{
	m_parryTimer += deltaTime;

	// パリィの受付時間の受付時間が過ぎたらfalseにしてリターン
	if (m_parryTimer >= m_pParam->m_parryWindow)
	{
		m_isParryActive = false;
		return;
	}

	m_isParryActive = true;
}

