#pragma once

class WayPoint;

class StageObject;

class WayPointManager
{
public:

	void Init();

	// WayPointの登録・削除
	bool RegisterWayPoint(const std::shared_ptr<WayPoint>& wayPoint);
	bool RemoveWayPoint(int id);

	void RemoveByOwner(const StageObject* owner);

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

	
	// WayPointを初期化する
	std::shared_ptr<WayPoint> CreateWayPoint();

	// デバッグ表示
	void DrawDebug();

	// デバッグの表示切り替えフラグ
	bool IsDebug() { return m_isDebug; }
	void SetIsDebug(const bool flg) { m_isDebug = flg; }

	bool Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner);

	bool Load(const std::string& filePath, const std::shared_ptr<StageObject>& owner);

	const std::vector<std::shared_ptr<WayPoint>>& GetWayPoints() const
	{
		return m_spWayPointList;
	}

	// Stage(親)のワールド位置・大きさを、管理している全WayPointに反映する
	void SetStageTransform(const StageObject* stageObject,const Math::Vector3& stagePos, const Math::Vector3& stageScale);

	// 全WayPointのエリアIDを座標から自動で決める
	void AssignAreaIDs();

	void UpdateFlowGoal(const int goalID);

	// 次に向かうWayPointのID
	int GetNextWayPointID(const int currentID)const;

	bool IsFlowGoal(const int id)const { return  m_flowGoalID == id; }

private:

	// 現在使われていない最小のIDを返す
	int FindAvailableID() const;

	// 標識を作る(ダイクストラ法)
	void BuildFlowField(const int goalID);

private:

	// 現在のステージに配置されているWayPointを所有する
	std::vector<std::shared_ptr<WayPoint>> m_spWayPointList;
	// バックアップ用のWayPointリスト
	std::vector<std::shared_ptr<WayPoint>>m_spBackupWayPointList;

	// ゴールまでの道のり(標識)
	std::unordered_map<int, float>m_costToGoal;

	// 今の標識がどのゴールのものか
	int m_flowGoalID = -1;

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
