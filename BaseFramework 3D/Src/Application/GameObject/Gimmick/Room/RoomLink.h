#pragma once

#include"Room.h"
#include"../../../Scene/SceneManager.h"

//==================================================================
// 「どの部屋に対応するか」を持つ小さな部品
//
// 部屋の名前を保存し、実行中は名前から部屋を探して返す。
// Inspectorの選択欄・セーブ/ロードもここにまとめてある。
//
// 使い方
//   m_roomLink.DrawCombo();                 // Inspectorで部屋を選ぶ
//   auto room = m_roomLink.GetRoom();       // 紐づけた部屋(無ければnullptr)
//   m_roomLink.SaveData(json);              // SaveDataの中で
//   m_roomLink.LoadData(json);              // LoadDataの中で
//==================================================================
class RoomLink
{
public:

	// 部屋を紐づけているか(名前が入っているか)
	bool HasLink()const { return !m_roomName.empty(); }

	const std::string& GetRoomName()const { return m_roomName; }

	// 紐づけた部屋を返す(紐づけていない・見つからない時はnullptr)
	std::shared_ptr<Room> GetRoom()
	{
		if (m_roomName.empty())
		{
			return nullptr;
		}

		auto spRoom = m_wpRoom.lock();

		if (spRoom)
		{
			return spRoom;
		}

		// 見失っていたら、名前で探し直す
		spRoom = FindRoomByName(m_roomName);

		if (spRoom)
		{
			m_wpRoom = spRoom;
			return spRoom;
		}

		// 見つからないときは、ログを1回だけ出す
		if (!m_warnedMissingRoom)
		{
			KdDebugGUI::Instance().AddLog("RoomLink : linked room not found : %s\n", m_roomName.c_str());

			m_warnedMissingRoom = true;
		}

		return nullptr;
	}

	// Inspectorで部屋を選ぶ。変更されたらMarkDirty()して、trueを返す
	bool DrawCombo(const char* label = "LinkedRoom")
	{
		bool changed = false;

		const char* preview = m_roomName.empty() ? "(None)" : m_roomName.c_str();

		if (ImGui::BeginCombo(label, preview))
		{
			if (ImGui::Selectable("(None)", m_roomName.empty()))
			{
				if (!m_roomName.empty())
				{
					m_roomName.clear();
					m_wpRoom.reset();
					m_warnedMissingRoom = false;

					changed = true;
				}
			}

			for (const auto& obj : SceneManager::Instance().GetObjList())
			{
				auto room = std::dynamic_pointer_cast<Room>(obj);

				if (!room)
				{
					continue;
				}

				const std::string name = room->GetObjectName();

				if (ImGui::Selectable(name.c_str(), name == m_roomName))
				{
					if (name != m_roomName)
					{
						m_roomName = name;
						m_wpRoom = room;
						m_warnedMissingRoom = false;

						changed = true;
					}
				}
			}

			ImGui::EndCombo();
		}

		if (changed)
		{
			EditorManager::Instance().MarkDirty();
		}

		return changed;
	}

	void SaveData(nlohmann::json& json)const
	{
		json["LinkedRoom"] = m_roomName;
	}

	void LoadData(const nlohmann::json& json)
	{
		if (json.contains("LinkedRoom"))
		{
			m_roomName = json["LinkedRoom"].get<std::string>();
		}

		m_wpRoom.reset();
		m_warnedMissingRoom = false;
	}

private:

	// 名前から部屋を探す
	static std::shared_ptr<Room> FindRoomByName(const std::string& name)
	{
		for (const auto& obj : SceneManager::Instance().GetObjList())
		{
			auto room = std::dynamic_pointer_cast<Room>(obj);

			if (room && room->GetObjectName() == name)
			{
				return room;
			}
		}

		return nullptr;
	}

	std::string m_roomName = {};
	std::weak_ptr<Room> m_wpRoom;
	bool m_warnedMissingRoom = false;
};
