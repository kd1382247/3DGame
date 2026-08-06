#pragma once

class WayPoint;

class WayPointManager
{
public:

	WayPointManager(){}
	~WayPointManager(){}

	// 登録・解除
	void Register(const std::shared_ptr<WayPoint>& point);
	void Unregister(int id);
	void Clear();

	// 検索
	std::shared_ptr<WayPoint>FindById(int id)const;
	std::shared_ptr<WayPoint>FindNearest(const Math::Vector3& pos)const;

	// 接続
	bool Connect(int idA, int idB, bool bidirectional = true);
	bool Disconnect(int idA, int idB, bool bidirecttional = true);

	// 経路探索
	std::vector<int>FindPath(int startId, int goalId)const;


	// デバッグ表示
	void DrawDebug();

private:

	struct SearchNode
	{
		int wayPointId = -1;

		float g = 0.0f;
		float h = 0.0f;
		float f = 0.0f;

		int parentId = -1;
	};

	float CalculateHeuristic(const WayPoint& current, const WayPoint& goal)const;

	std::vector<int>ReconstructPath(const std::unordered_map<int, SearchNode>& searchNodes, int goalId)const;



	// 無効になったweak_ptrを削除
	void RemoveExpired();

private:

	std::vector<std::weak_ptr<WayPoint>>m_wpWayPoints;

};