#include "CheckPointManager.h"

void CheckPointManager::Reset()
{
	m_reachedOrder = kNoOrder;
	m_respawnPos = Math::Vector3::Zero;
	m_playTime = 0.0f;
	m_clearedRoomNames.clear();
}

bool CheckPointManager::TryReach(const int order, const Math::Vector3& pos)
{
	// 今の記録より大きい順番だけ記録する(同じ順番・小さい順番は無視)
	if (order <= m_reachedOrder)
	{
		return false;
	}

	m_reachedOrder = order;
	m_respawnPos = pos;

	return true;
}

void CheckPointManager::SaveProgress(const float playTime, const std::vector<std::string>& clearedRoomNames)
{
	m_playTime = playTime;

	m_clearedRoomNames.clear();
	m_clearedRoomNames.insert(clearedRoomNames.begin(), clearedRoomNames.end());
}
