#pragma once

class WayPoint;

class WayPointManager
{
public:

	void Init();

	// WayPointの登録・削除
	bool RegisterWayPoint(const std::shared_ptr<WayPoint>& wayPoint);
	bool RemoveWayPoint(int id);

	void ClearWayPoints();
	// ウェイポイントを復元
	void RestoreWayPoints();
	// バックアップをクリア
	void ClearBackup();

	// 指定IDが登録されていない場合はnullptrを返す
	std::shared_ptr<WayPoint> FindWayPoint(int id) const;

	// 引数の位置から近いWayPointを返す
	std::shared_ptr<WayPoint> FindNearest(const Math::Vector3& pos,const int areaID) const;

	// 接続
	bool Connect(int idA, int idB, bool bidirectional = true);
	// 接続解除
	bool Disconnect(int idA, int idB, bool bidirectional = true);

	// 経路探索
	std::vector<int> FindPath(int startId, int goalId) const;

	// WayPointを初期化する
	std::shared_ptr<WayPoint> CreateWayPoint();

	// デバッグ表示
	void DrawDebug();

	// デバッグの表示切り替えフラグ
	bool IsDebug() { return m_isDebug; }
	void SetDebugFlg(const bool flg) { m_isDebug = flg; }

	bool Save(const std::string& filePath);

	bool Load(const std::string& filePath);

	const std::vector<std::shared_ptr<WayPoint>>& GetWayPoints() const
	{
		return m_spWayPoints;
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

	// 現在使われていない最小のIDを返す
	int FindAvailableID() const;

	// 現在地点からゴール地点までの予想距離を計算
	float CalculateHeuristic(const WayPoint& current, const WayPoint& goal) const;

	// スタートからゴールまでの道のり距離を返す
	std::vector<int> ReconstructPath(
		const std::unordered_map<int, SearchNode>& searchNodes,
		int goalId) const;

private:

	// 現在のステージに配置されているWayPointを所有する
	std::vector<std::shared_ptr<WayPoint>> m_spWayPoints;
	// バックアップ用のWayPointリスト
	std::vector<std::shared_ptr<WayPoint>>m_spBackupWayPoints;

	// デバッグ
	std::unique_ptr<KdDebugWireFrame> m_pDebugWire = nullptr;

	bool m_isDebug = false;

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
