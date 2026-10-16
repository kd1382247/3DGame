#pragma once

class Room;

class RoomManager
{
public:

	void EnterRoom(const std::shared_ptr<Room>& room);
	// 今の部屋と同じ部屋の場合は参照を解除
	void ExitRoom(const std::shared_ptr<Room>& room);

	std::shared_ptr<Room>GetCurrentRoom()const { return m_wpCurrentRoom.lock(); }

	bool IsCurrentRoom(const Room* room)const
	{
		return m_wpCurrentRoom.lock().get() == room;
	}

	// 戦闘中かどうか
	bool IsCombat()const;

	bool IsBossCombat()const;

private:


	std::weak_ptr<Room>m_wpCurrentRoom;

private:

	RoomManager(){}
	~RoomManager(){}

public:

	static RoomManager& Instance()
	{
		static RoomManager instance;
		return instance;
	}
};