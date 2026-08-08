#pragma once

class WayPoint;

class WayPointManager
{
public:

	void Init();



	// 登録・解除
	void RegisterWayPoint(const std::shared_ptr<WayPoint>& point);
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


	const std::vector<std::weak_ptr<WayPoint>>& GetWayPoints()const
	{
		return m_wpWayPoints;
	}

private:

	struct SearchNode
	{
		int wayPointId = -1; // どのWayPointの探索情報なのか

		float g = 0.0f;      // スタートから進んだ距離
		float h = 0.0f;      // ゴールまでの予想距離
		float f = 0.0f;      // gとhの合計値でゴールまでの経路の長さを示す

		int parentId = -1;   // 経路を復元するための親(0番の親-1 → 1番の親0)
	};

	// 現在地点からゴール地点までの予想距離を計算
	float CalculateHeuristic(const WayPoint& current, const WayPoint& goal)const;

	// スタートからゴールまでの道のり距離を返す
	std::vector<int>ReconstructPath(const std::unordered_map<int, SearchNode>& searchNodes, int goalId)const;
	// 無効になったweak_ptrを削除
	void RemoveExpired();

private:

	std::vector<std::weak_ptr<WayPoint>>m_wpWayPoints;

	// デバッグ
	std::unique_ptr<KdDebugWireFrame>m_pDebugWire = nullptr;


private:

	WayPointManager() { Init(); }
	~WayPointManager() {}

public:

	static WayPointManager& Instance()
	{
		static WayPointManager instance;
		return instance;
	}

};