#include "WayPointManager.h"

#include"WayPoint/WayPoint.h"

void WayPointManager::Register(const std::shared_ptr<WayPoint>& point)
{
	if (!point)
	{
		return;
	}

	// 同じIDが既に登録されていないか確認
	if (FindById(point->GetId()))
	{
		return;
	}

	m_wpWayPoints.emplace_back(point);

}

void WayPointManager::Unregister(int id)
{}

void WayPointManager::Clear()
{}

std::shared_ptr<WayPoint> WayPointManager::FindById(int id) const
{
	for (const auto& weakPoint : m_wpWayPoints)
	{
		auto point = weakPoint.lock();

		if (!point)
		{
			continue;
		}

		if (point->GetId() == id)
		{
			return point;
		}
	}

	return nullptr;
}

std::shared_ptr<WayPoint> WayPointManager::FindNearest(const Math::Vector3& pos) const
{
	std::shared_ptr<WayPoint>nearest = nullptr;
	float nearestDistanceSq = FLT_MAX;


	for (const auto& weakPoint : m_wpWayPoints)
	{
		auto point = weakPoint.lock();

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
	auto pointA = FindById(idA);
	auto pointB = FindById(idB);

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

bool WayPointManager::Disconnect(int idA, int idB, bool bidirecttional)
{
	return false;
}

std::vector<int> WayPointManager::FindPath(int startId, int goalId) const
{
	// スタート地点とゴール地点のWayPointを探す
	auto startPoint = FindById(startId);
	auto goalPoint = FindById(goalId);

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

	startNode.wayPointId =startId ;
	startNode.g = 0.0f;
	startNode.h = CalculateHeuristic(*startPoint,*goalPoint);


	startNode.f = startNode.g + startNode.h;
	startNode.parentId = -1;


	std::vector<int>openList;

	std::unordered_set<int>closeList;

	std::unordered_map<int, SearchNode>searchNodes;


	searchNodes[startId] = startNode;
	openList.push_back(startId);

	while (!openList.empty())
	{
		// Fが最小のIDを探す
		auto bestIt = std::min_element(openList.begin(), openList.end(),
			[&](int leftId, int rightId)
			{
				return searchNodes.at(leftId).f < searchNodes.at(rightId).f;
			});

		int currentId = *bestIt;


		openList.erase(bestIt);
		closeList.insert(currentId);

		if (currentId ==goalId )
		{
			return ReconstructPath(searchNodes, goalId);
		}

		auto currentPoint = FindById(currentId);

		if (!currentPoint)
		{
			continue;
		}

		for (int linkId : currentPoint->GetLinks())
		{
			if (closeList.contains(linkId))
			{
				continue;
			}

			auto linkedPoint = FindById(linkId);

			if (!linkedPoint)
			{
				continue;
			}


			Math::Vector3 difference = linkedPoint->GetPos() - currentPoint->GetPos();

			float moveCost = difference.Length();

			// スタート地点から、接続先WayPointまで進んだ場合の合計距離
			float newG = searchNodes.at(currentId).g + moveCost;

			// 新しいノードか確認する
			bool isNewNode = !searchNodes.contains(linkId);

			bool isShorterPath = !isNewNode && newG < searchNodes.at(linkId).g;


			if (!isNewNode && !isShorterPath)
			{
				continue;
			}

			SearchNode linkedSearchNode;

			linkedSearchNode.wayPointId = linkId;

			linkedSearchNode.g = newG;
			linkedSearchNode.h = CalculateHeuristic(*linkedPoint, *goalPoint);

			linkedSearchNode.f = linkedSearchNode.g + linkedSearchNode.h;

			linkedSearchNode.parentId = currentId;

			searchNodes[linkId] = linkedSearchNode;

			if (std::find(openList.begin(), openList.end(), linkId) == openList.end())
			{
				openList.push_back(linkId);
			}


		}
	}

	return{};


}

void WayPointManager::DrawDebug()
{}

float WayPointManager::CalculateHeuristic(const WayPoint& current, const WayPoint& goal) const
{
	// スタートからゴールまでの予想距離を計算
	Math::Vector3 difference = goal.GetPos() - current.GetPos();

	return difference.Length();
}

std::vector<int> WayPointManager::ReconstructPath(const std::unordered_map<int, SearchNode>& searchNodes, int goalId) const
{

	std::vector<int>path;

	int currentId = goalId;

	while (currentId!=-1)
	{
		path.push_back(currentId);

		auto nodeIt = searchNodes.find(currentId);

		if (nodeIt == searchNodes.end())
		{
			return{};
		}

		currentId = nodeIt->second.parentId;

	}

	std::reverse(path.begin(), path.end());

	return path;


}

void WayPointManager::RemoveExpired()
{}
