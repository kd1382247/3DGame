#include "WayPointManager.h"

#include"WayPoint/WayPoint.h"

void WayPointManager::RegisterWayPoint(const std::shared_ptr<WayPoint>& point)
{
	if (!point)
	{
		return;
	}

	// 同じIDが既に登録されていないか確認
	if (FindByID(point->GetID()))
	{
		return;
	}

	m_wpWayPoints.push_back(point);

}

void WayPointManager::Unregister(int id)
{}

void WayPointManager::Clear()
{
	//m_wpWayPoints.clear();
}

std::shared_ptr<WayPoint> WayPointManager::FindByID(int id) const
{
	// 指定されたIDを検索、見つかればそのWayPointを返す
	for (const auto& weakPoint : m_wpWayPoints)
	{
		auto point = weakPoint;

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

std::shared_ptr<WayPoint> WayPointManager::FindNearest(const Math::Vector3& pos) const
{
	// 調べた中で一番近いWayPointを保存
	std::shared_ptr<WayPoint>nearest = nullptr;

	// 今までで一番短かった距離を保存
	float nearestDistanceSq = FLT_MAX;


	// 1個ずつWayPointを確認する
	for (const auto& weakPoint : m_wpWayPoints)
	{
		auto point = weakPoint;

		if (!point)
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
	auto pointA = FindByID(idA);
	auto pointB = FindByID(idB);

	if (!pointA || !pointB)
	{
		return false;
	}

	if (idA == idB)
	{
		return false;
	}

	pointA->AddLink(idB);
	if (bidirectional)
	{
		pointB->AddLink(idA);
	}

	return true;

}

bool WayPointManager::Disconnect(int idA, int idB, bool bidirectional)
{
	auto pointA = FindByID(idA);
	auto pointB = FindByID(idB);

	if (!pointA || !pointB)
	{
		return false;
	}

	if (idA == idB)
	{
		return false;
	}

	pointA->RemoveLink(idB);
	if (bidirectional)
	{
		pointB->RemoveLink(idA);
	}

	return true;
}

std::vector<int> WayPointManager::FindPath(int startId, int goalId) const
{
	// スタート地点とゴール地点のWayPointを探す
	auto startPoint = FindByID(startId);
	auto goalPoint = FindByID(goalId);

	// どちらかのWayPointがなければreturn
	if (!startPoint || !goalPoint)
	{
		return{};
	}
	// スタートIDとゴールIDが同じであればスタートIDを返す
	if (startId == goalId)
	{
		return { startId };
	}

	SearchNode startNode;
	// スタート地点のWayPointを入れる
	startNode.wayPointId =startId ;
	startNode.g = 0.0f;
	startNode.h = CalculateHeuristic(*startPoint,*goalPoint);

	startNode.f = startNode.g + startNode.h;
	startNode.parentId = -1;

	// これから調べる候補
	std::vector<int>openList;

	// 調べ終わった場所を保存
	std::unordered_set<int>closeList;

	// 各WayPointの探索情報
	std::unordered_map<int, SearchNode>searchNodes;

	searchNodes[startId] = startNode;
	openList.push_back(startId);

	while (!openList.empty())
	{
		// Fが最小のIDのイテレータを返す処理
		auto bestIt = std::min_element(openList.begin(), openList.end(),
			[&](int leftId, int rightId)
			{  // Fが小さいWayPointを比較する
				return searchNodes.at(leftId).f < searchNodes.at(rightId).f;
			});
		
		// これから調べるWayPointのID
		int currentId = *bestIt;

		// 調べる候補から削除
		openList.erase(bestIt);
		// 調べ終わったリストに追加
		closeList.insert(currentId);

		// ゴールのIDと同じか
		if (currentId ==goalId )
		{
			// 
			return ReconstructPath(searchNodes, goalId);
		}

		auto currentPoint = FindByID(currentId);

		if (!currentPoint)
		{
			continue;
		}

		// 現在のWayPointから、その接続先のWayPointを調べる
		for (int linkId : currentPoint->GetLinks())
		{
			// 調査済みリストに入っていたらスキップ
			if (closeList.contains(linkId))
			{
				continue;
			}

			auto linkedPoint = FindByID(linkId);

			if (!linkedPoint)
			{
				continue;
			}

			// 接続先との距離を計算
			Math::Vector3 difference = linkedPoint->GetPos() - currentPoint->GetPos();
			// 一区間の距離を入れる
			float moveCost = difference.Length();

			// スタート地点から、接続先WayPointまで進んだ合計距離
			float newG = searchNodes.at(currentId).g + moveCost;

			// 接続先WayPointを、今回の探索で初めて発見したか？
			bool isNewNode = !searchNodes.contains(linkId);

			// 発見済みの場合
			// 今回見つけた道の方が、以前の道より短いか?
			bool isShorterPath = !isNewNode && newG < searchNodes.at(linkId).g;


			// 初めて発見してない、前より短い経路でもない場合スキップ
			if (!isNewNode && !isShorterPath)
			{
				continue;
			}

			// 接続先のWayPoint情報
			SearchNode linkedSearchNode;
			
			// 今調べている接続先IDを入れる
			linkedSearchNode.wayPointId = linkId;

			// スタート地点から接続先までの距離
			linkedSearchNode.g = newG;

			// 接続先からゴールまでの予想距離
			linkedSearchNode.h = CalculateHeuristic(*linkedPoint, *goalPoint);

			linkedSearchNode.f = linkedSearchNode.g + linkedSearchNode.h;

			// 接続先の親ID = 現在のWayPointIDを入れる
			linkedSearchNode.parentId = currentId;

			// 接続先Nodeを新たに追加
			searchNodes[linkId] = linkedSearchNode;

			// このWayPointがまだオープンリストに入っていなければ追加
			if (std::find(openList.begin(), openList.end(), linkId) == openList.end())
			{
				openList.push_back(linkId);
			}
		}
	}

	return{};


}

std::shared_ptr<WayPoint> WayPointManager::CreateWayPoint()
{
	std::shared_ptr<WayPoint>wayPoint = std::make_shared<WayPoint>();

	wayPoint->SetID(GetNextWayPointID());
	wayPoint->SetPos({ 0.0f,0.0f,0.0f });

	// オブジェクトの名前をセット 後ろにID
	std::string objName = "WayPoint_" + std::to_string(GetNextWayPointID());
	wayPoint->SetObjectName(objName);

	RegisterWayPoint(wayPoint);

	return wayPoint;

}

int WayPointManager::GetNextWayPointID() const
{
	int maxID = -1;

	for (const auto& wayPoint : m_wpWayPoints)
	{
		if (!wayPoint)
		{
			continue;
		}

		maxID = std::max(maxID, wayPoint->GetID());
	}

	return maxID + 1;
}

void WayPointManager::Init()
{
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

void WayPointManager::DrawDebug()
{
	
	for (const auto& weakPoint : m_wpWayPoints)
	{
		auto point = weakPoint;

		if (!point)
		{
			continue;
		}

		// WayPointの位置を表す球を表示
		weakPoint->DrawDebug();

		// 接続関係を表す線を描画
		for (int linkId : point->GetLinks())
		{
			auto linkedPoint = FindByID(linkId);

			if (!linkedPoint)
			{
				continue;
			}
			// 小さいID側だけ登録
			if (point->GetID() > linkId)
			{
				continue;
			}

			m_pDebugWire->AddDebugLine(point->GetPos(), linkedPoint->GetPos(), kBlueColor);
		}
	}

	m_pDebugWire->Draw();
}

float WayPointManager::CalculateHeuristic(const WayPoint& current, const WayPoint& goal) const
{
	
	Math::Vector3 difference = goal.GetPos() - current.GetPos();

	return difference.Length();
}

std::vector<int> WayPointManager::ReconstructPath(const std::unordered_map<int, SearchNode>& searchNodes, int goalId) const
{
	// スタートからゴールまでのWayPointのID一覧
	std::vector<int>path;

	// ゴールから始める
	int currentId = goalId;

	// 親が無くなるまでたどる
	while (currentId!=-1)
	{
		path.push_back(currentId);

		// 現在のIDのscerchNodesを探す
		auto nodeIt = searchNodes.find(currentId);


		if (nodeIt == searchNodes.end())
		{
			return{};
		}

		// 親情報を更新
		currentId = nodeIt->second.parentId;

	}

	// ゴールから親を逆にたどり、それを反転して
	// 敵が実際に歩く順番へ直す
	std::reverse(path.begin(), path.end());

	return path;
}

void WayPointManager::RemoveExpired()
{}
