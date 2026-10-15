#include "RoomManager.h"

#include"Room.h"

void RoomManager::EnterRoom(const std::shared_ptr<Room>& room)
{

	m_wpCurrentRoom = room;

	KdDebugGUI::Instance().AddLog("Enter Room\n");

}

void RoomManager::ExitRoom(const std::shared_ptr<Room>&room)
{
	if (m_wpCurrentRoom.lock() == room)
	{
		m_wpCurrentRoom.reset();

		KdDebugGUI::Instance().AddLog("Exit Room\n");
	}
}

bool RoomManager::IsCombat() const
{
	auto spRoom = m_wpCurrentRoom.lock();

	if (!spRoom)
	{
		return false;
	}

	return spRoom->IsCombat();
}
