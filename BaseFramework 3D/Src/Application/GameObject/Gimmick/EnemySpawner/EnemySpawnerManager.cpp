#include "EnemySpawnerManager.h"

#include"EnemySpawner.h"

#include"../Room/Room.h"

#include"../../../Scene/SceneManager.h"

bool EnemySpawnerManager::RequestSpawn(const Math::Vector3& requesterPos, const EnemySpawnerRequest& request)
{
	// 合図を出した場所にある部屋を探す
	std::shared_ptr<Room> spRoom = nullptr;

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto room = std::dynamic_pointer_cast<Room>(obj);

		if (room && room->IsInside(requesterPos))
		{
			spRoom = room;
			break;
		}
	}

	if (!spRoom)
	{
		KdDebugGUI::Instance().AddLog("EnemySpawnerManager : room not found at requester position\n");
		return false;
	}

	// その部屋に紐づいていて、要求を受けられるスポナーを集める
	std::vector<std::shared_ptr<EnemySpawner>> spawners;

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto spawner = std::dynamic_pointer_cast<EnemySpawner>(obj);

		if (!spawner)
		{
			continue;
		}

		if (spawner->GetLinkRoomName() != spRoom->GetObjectName())
		{
			continue;
		}

		if (spawner->CanAcceptRequest())
		{
			spawners.push_back(spawner);
		}
	}

	if (spawners.empty())
	{
		KdDebugGUI::Instance().AddLog("EnemySpawnerManager : no spawner can accept the request : %s\n", spRoom->GetObjectName().c_str());
		return false;
	}

	// 数をスポナーの数で分ける(割り切れない分は、前のスポナーから1体ずつ多く出す)
	const int spawnerCount = static_cast<int>(spawners.size());

	const int baseCount = request.count / spawnerCount;
	const int remainder = request.count % spawnerCount;

	for (int i = 0; i < spawnerCount; i++)
	{
		EnemySpawnerRequest divided = request;

		divided.count = baseCount + (i < remainder ? 1 : 0);

		if (divided.count <= 0)
		{
			continue;
		}

		spawners[i]->RequestSpawn(divided);
	}

	return true;
}
