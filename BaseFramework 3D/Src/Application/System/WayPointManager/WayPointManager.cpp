#include "WayPointManager.h"

#include"../../GameObject/WayPoint/WayPoint.h"

#include"../../Editor/EditorManager.h"

#include"../CollisionManager/CollisionManager.h"
#include"../../GameObject/Gimmick/AreaVolume/AreaVolume.h"

#include<queue>
#include<functional>

bool WayPointManager::RegisterWayPoint(const std::shared_ptr<WayPoint>& wayPoint)
{
	// 無効なWayPointは登録しない
	if (!wayPoint)
	{
		return false;
	}

	// IDはWayPointを識別するため、一意でなければならない
	if (FindWayPoint(wayPoint->GetID()))
	{
		return  false;
	}

	m_spWayPointList.push_back(wayPoint);

	// 標識が古くなったので初期化
	m_flowGoalID = -1;
	
	return true;
}

bool WayPointManager::RemoveWayPoint(int id)
{
	auto target = FindWayPoint(id);

	if (!target)
	{
		return false;
	}

	// 削除後に存在しないIDへのリンクが残らないよう、
	// 管理リストから消す前に全WayPointの参照を解除する
	for (const auto& wayPoint : m_spWayPointList)
	{
		if (!wayPoint)
		{
			continue;
		}

		wayPoint->RemoveLink(id);
	}

	auto it = std::find(m_spWayPointList.begin(), m_spWayPointList.end(), target);

	if (it == m_spWayPointList.end())
	{
		return false;
	}

	m_spWayPointList.erase(it);

	// 標識が古くなったので初期化
	m_flowGoalID = -1;

	return true;
}

void WayPointManager::RemoveByOwner(const StageObject* owner)
{
	// 先に消すIDを集める(ループ中にリストを変更しないため)
	std::vector<int>removeIDs;

	for (const auto& point : m_spWayPointList)
	{
		if (point && point->GetOwner().get() == owner)
		{
			removeIDs.push_back(point->GetID());
		}
	}

	for (int id : removeIDs)
	{
		RemoveWayPoint(id);
	}
}

void WayPointManager::ClearWayPoints()
{
	// 一時的にバックアップリストに移す
	m_spBackupWayPointList = std::move(m_spWayPointList);
	m_spWayPointList.clear();

	// 標識が古くなったので初期化
	m_flowGoalID = -1;
}

void WayPointManager::RestoreWayPoints()
{
	// バックアップリストから復元
	m_spWayPointList = std::move(m_spBackupWayPointList);
	m_spBackupWayPointList.clear();


	// 標識が古くなったので初期化
	m_flowGoalID = -1;
}

void WayPointManager::ClearBackup()
{
	m_spBackupWayPointList.clear();
}

std::shared_ptr<WayPoint> WayPointManager::FindWayPoint(int id) const
{
	// 指定されたIDを検索、見つかればそのWayPointを返す
	for (const auto& point : m_spWayPointList)
	{
		if (!point)
		{
			continue;
		}

		if (point->GetID() == id)
		{
			return point;
		}
	}

	return nullptr;
}

std::shared_ptr<WayPoint> WayPointManager::FindNearest(const Math::Vector3& pos, const int areaID) const
{

	if (areaID < 0)
	{
		return nullptr;
	}

	// 調べた中で一番近いWayPointを保存
	std::shared_ptr<WayPoint>nearest = nullptr;

	// 今までで一番短かった距離を保存
	float nearestDistanceSq = FLT_MAX;


	// 1個ずつWayPointを確認する
	for (const auto& point : m_spWayPointList)
	{
		if (!point)
		{
			continue;
		}

		// 違うエリアは候補にしない
		if (areaID != point->GetAreaID())
		{
			continue;
		}

		// wayPointの位置  -  調べたい位置
		Math::Vector3 difference = point->GetPos() - pos;


		float distanceSq = difference.LengthSquared();

		// 調べたい位置と近ければ記録を更新
		if (distanceSq < nearestDistanceSq)
		{
			nearestDistanceSq = distanceSq;
			nearest = point;
		}
	}

	return nearest;
}

bool WayPointManager::Connect(int idA, int idB, bool bidirectional)
{
	auto pointA = FindWayPoint(idA);
	auto pointB = FindWayPoint(idB);

	if (!pointA || !pointB)
	{
		return false;
	}

	if (idA == idB)
	{
		return false;
	}

	const bool addedA = pointA->AddLink(idB);
	bool addedB = false;

	if (bidirectional)
	{
		addedB = pointB->AddLink(idA);
	}

	// 標識が古くなったので初期化
	m_flowGoalID = -1;

	return addedA || addedB;

}

bool WayPointManager::Disconnect(int idA, int idB, bool bidirectional)
{
	auto pointA = FindWayPoint(idA);
	auto pointB = FindWayPoint(idB);

	if (!pointA || !pointB)
	{
		return false;
	}

	if (idA == idB)
	{
		return false;
	}

	const bool hadLinkA = std::find(
		pointA->GetLinks().begin(), pointA->GetLinks().end(), idB)
		!= pointA->GetLinks().end();

	pointA->RemoveLink(idB);

	bool hadLinkB = false;
	if (bidirectional)
	{
		hadLinkB = std::find(
			pointB->GetLinks().begin(), pointB->GetLinks().end(), idA)
			!= pointB->GetLinks().end();

		pointB->RemoveLink(idA);
	}

	// 標識が古くなったので初期化
	m_flowGoalID = -1;

	return hadLinkA || hadLinkB;
}

std::shared_ptr<WayPoint> WayPointManager::CreateWayPoint()
{
	const int id = FindAvailableID();

	std::shared_ptr<WayPoint>wayPoint = std::make_shared<WayPoint>();


	wayPoint->SetID(id);

	// オブジェクトの名前をセット 後ろにID
	std::string objName = "WayPoint_" + std::to_string(id);
	wayPoint->SetObjectName(objName);

	if (!RegisterWayPoint(wayPoint))
	{
		return nullptr;
	}

	return wayPoint;

}

int WayPointManager::FindAvailableID() const
{
	int id = 0;

	while (FindWayPoint(id))
	{
		++id;
	}

	return id;
}

void WayPointManager::Init()
{
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

void WayPointManager::DrawDebug()
{
	if (!IsDebug())
	{
		return;
	}

	for (const auto& point : m_spWayPointList)
	{
		if (!point)
		{
			continue;
		}

		// 持ち主が違う場合表示しない
		if (point->GetOwner() != EditorManager::Instance().GetActiveStage())
		{
			continue;
		}

		// WayPointの位置を表す球を表示
		point->DrawDebug();

		// 接続関係を表す線を描画
		for (int linkId : point->GetLinks())
		{
			auto linkedPoint = FindWayPoint(linkId);

			if (!linkedPoint)
			{
				continue;
			}
			// 小さいID側だけ登録
			if (point->GetID() > linkId)
			{
				continue;
			}

			m_pDebugWire->AddDebugLine(point->GetPos(), linkedPoint->GetPos(), kGreenColor);
		}
	}

	m_pDebugWire->Draw();
}

bool WayPointManager::Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner)
{

	nlohmann::json rootJson;
	rootJson["WayPoints"] = nlohmann::json::array();

	for (const auto& wayPoint : m_spWayPointList)
	{
		if (!wayPoint||
			wayPoint->GetOwner()!=owner)
		{
			continue;
		}

		nlohmann::json wayPointJson;

		wayPointJson["ID"] = wayPoint->GetID();

		wayPointJson["Name"] = wayPoint->GetObjectName();

		// 座標(Stageからのローカル位置)
		const auto& pos = wayPoint->GetLocalPos();

		wayPointJson["Position"]["x"] = pos.x;
		wayPointJson["Position"]["y"] = pos.y;
		wayPointJson["Position"]["z"] = pos.z;

		wayPointJson["Links"] = wayPoint->GetLinks();

		rootJson["WayPoints"].push_back(wayPointJson);
	}

	std::ofstream file(filePath);

	if (!file.is_open())
	{
		KdDebugGUI::Instance().AddErrorLog("WayPointDataの保存に失敗しました\n");

		return false;
	}

	file << rootJson.dump(4);

	return true;
}

bool WayPointManager::Load(const std::string& filePath, const std::shared_ptr<StageObject>& owner)
{

	std::ifstream file(filePath);

	if (!file.is_open())
	{
		KdDebugGUI::Instance().AddErrorLog("WayPointDataを開けませんでした\n");

		return false;
	}

	nlohmann::json rootJson;

	try
	{
		file >> rootJson;
	}
	catch (const nlohmann::json::exception& e)
	{
		KdDebugGUI::Instance().AddErrorLog("WayPointDataの読み込みに失敗しました\n");

		KdDebugGUI::Instance().AddErrorLog("%s\n", e.what());

		return false;
	}

	if (!rootJson.contains("WayPoints") ||
		!rootJson["WayPoints"].is_array())
	{
		KdDebugGUI::Instance().AddErrorLog("WayPoints配列がありません\n");

		return false;
	}

	// ファイルのIDをキー、新たに割り振ったIDを値に入れる
	std::unordered_map<int, int>idMap;

	// Jsonに保存されてる情報でWayPointを生成
	for (const auto& wpJson : rootJson["WayPoints"])
	{

		auto obj = KdGameObjectFactory::Instance().CreateGameObject("WayPoint");

		auto wayPoint = std::dynamic_pointer_cast<WayPoint>(obj);

		if (!wayPoint)
		{
			continue;
		}

		wayPoint->Init();
		// ID
		wayPoint->SetID(FindAvailableID());

		idMap[wpJson["ID"].get<int>()] = wayPoint->GetID();

		//Name
		wayPoint->SetObjectName("WayPoint_" + std::to_string(wayPoint->GetID()));
		//Position(Stageからのローカル位置)
		wayPoint->SetLocalPos({
			wpJson["Position"]["x"].get<float>(),
			wpJson["Position"]["y"].get<float>(),
			wpJson["Position"]["z"].get<float>()
			});

		wayPoint->SetOwner(owner);


		// WayPointsに登録
		if (!RegisterWayPoint(wayPoint))
		{
			return false;
		}

	}

	// 接続設定
	for (auto& wpJson : rootJson["WayPoints"])
	{
		int id = wpJson["ID"].get<int>();

		if (!idMap.contains(id))
		{
			continue;
		}

		// IDからWayPointを探す
		auto wayPoint = FindWayPoint(idMap[id]);

		if (!wayPoint)
		{
			continue;
		}

		for (const auto& linkJson : wpJson["Links"])
		{
			int linkID = linkJson.get<int>();

			if (!idMap.contains(linkID))
			{
				continue;
			}

			wayPoint->AddLink(idMap[linkID]);
		}
	}

	// 標識が古くなったので初期化
	m_flowGoalID = -1;

	return true;
}

void WayPointManager::BuildFlowField(const int goalID)
{
	m_costToGoal.clear();

	m_flowGoalID = goalID;

	if (!FindWayPoint(goalID))
	{
		return;
	}

	m_costToGoal[goalID] = 0.0f; // 表 : ゴールは距離0と記録

	using QueueItem = std::pair<float, int>; //ゴールまでの距離、WayPointのID

	std::priority_queue
		<QueueItem,std::vector <QueueItem>,std::greater<QueueItem>> openQueue;

	openQueue.push({ 0.0f,goalID }); // 作業リスト : ゴールから調べ始める

	while (!openQueue.empty())
	{

		auto [cost, currentID] = openQueue.top();
		openQueue.pop();

		// 取り出した記録がcostToGoalより記録が大きければ処理を飛ばす
		if (cost > m_costToGoal[currentID])
		{
			continue;
		}

		auto wayPoint = FindWayPoint(currentID);

		if (!wayPoint)
		{
			continue;
		}

		for (int linkId : wayPoint->GetLinks())
		{
			auto linkedPoint = FindWayPoint(linkId);

			if (!linkedPoint)
			{
				continue;
			}

			//　隣までの距離
			float neighborDistance =(wayPoint->GetPos() - linkedPoint->GetPos()).Length();

			// 新しい距離
			float newDist = cost + neighborDistance;

			// 表があるか
			bool hasEntry = m_costToGoal.contains(linkId);

			bool IsShortPath = hasEntry && newDist<m_costToGoal.at(linkId);

			if (hasEntry && !IsShortPath)
			{
				continue;
			}

			m_costToGoal[linkId] = newDist;
			openQueue.push({ newDist,linkId });

		}
	}

}

void WayPointManager::SetStageTransform(const StageObject* stageObject,const Math::Vector3& stagePos, const Math::Vector3& stageScale)
{
	for (const auto& point : m_spWayPointList)
	{
		if (!point ||
			!point->ShouldFollow(stageObject))
		{
			continue;
		}

		point->SetStageTransform(stagePos, stageScale);
	}

	AssignAreaIDs();

	// 標識が古くなったので初期化
	m_flowGoalID = -1;
}

void WayPointManager::AssignAreaIDs()
{

	std::vector<std::shared_ptr<AreaVolume>> areaVolumeList;

	for (const auto& obj : CollisionManager::Instance().GetObjects(CollisionLayer::Area))
	{
		auto areaVolume = std::dynamic_pointer_cast<AreaVolume>(obj.lock());

		if (!areaVolume)
		{
			continue;
		}

		areaVolumeList.push_back(areaVolume);
	}


	for (const auto& wayPoint : m_spWayPointList)
	{
		if (!wayPoint)
		{
			continue;
		}

		int areaID = -1;

		for (const auto& areaVolume : areaVolumeList)
		{
			if (areaVolume->IsInside(wayPoint->GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f)))
			{
				areaID = areaVolume->GetAreaID();
				break;
			}
		}

		wayPoint->SetAreaID(areaID);

	}
}

void WayPointManager::UpdateFlowGoal(const int goalID)
{
	if (m_flowGoalID == goalID)
	{
		return;
	}

	BuildFlowField(goalID);

}

int WayPointManager::GetNextWayPointID(const int currentID) const
{
	
	auto wayPoint = FindWayPoint(currentID);

	if (!wayPoint)
	{
		return -1;
	}

	int   id = -1;
	float shortPath = FLT_MAX;

	for (int linkID : wayPoint->GetLinks())
	{

		if (!m_costToGoal.contains(linkID))
		{
			continue;
		}

		auto linkedPoint = FindWayPoint(linkID);

		if (!linkedPoint)
		{
			continue;
		}

		//　隣までの距離
		float neighborDistance = (wayPoint->GetPos() - linkedPoint->GetPos()).Length();

		float totalDist = neighborDistance + m_costToGoal.at(linkID);

		if (totalDist < shortPath)
		{
			shortPath = totalDist;
			id = linkID;
		}
	}

	return id;

}
