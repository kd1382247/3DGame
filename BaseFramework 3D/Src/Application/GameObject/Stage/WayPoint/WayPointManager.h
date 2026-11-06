#pragma once

class WayPoint;
class AreaVolume;
class StageObject;

class WayPointManager
{
public:

	void UnLinkID(int id);

	void RemoveByOwner(const StageObject* owner);

	// 指定IDが登録されていない場合はnullptrを返す
	std::shared_ptr<WayPoint> FindWayPoint(int id) const;

	// 引数の位置から近いWayPointを返す
	std::shared_ptr<WayPoint> FindNearest(const Math::Vector3& pos,const int areaID) const;

	// 接続
	bool Connect(int idA, int idB, bool bidirectional = true);
	// 接続解除
	bool Disconnect(int idA, int idB, bool bidirectional = true);

	
	// WayPointを初期化する
	std::shared_ptr<WayPoint> CreateWayPoint(const std::shared_ptr<StageObject>& owner);

	// デバッグの表示切り替えフラグ
	bool IsDebug() { return m_isDebug; }
	void SetIsDebug(const bool flg) { m_isDebug = flg; }

	bool Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner);

	bool Load(const std::string& filePath, const std::shared_ptr<StageObject>& owner);

	std::vector<std::shared_ptr<WayPoint>> GetWayPoints() const;

	std::vector<std::weak_ptr<AreaVolume>> GetAreaVolumes()const;

	void UpdateFlowGoal(const int goalID);

	// 次に向かうWayPointのID
	int GetNextWayPointID(const int currentID)const;

	bool IsFlowGoal(const int id)const { return  m_flowGoalID == id; }

	// 配列が古くなった
	void InvalidateIndex() { m_isIndexDirty = true; }

	// 表が古くなった
	void InvalidateFlowField() { m_flowGoalID = -1; }

private:

	void UpdateWayPointList()const;

	// 現在使われていない最小のIDを返す
	int FindAvailableID() const;

	// 標識を作る(ダイクストラ法)
	void BuildFlowField(const int goalID);

private:

	// ゴールまでの道のり(標識)
	std::unordered_map<int, float>m_costToGoal;

	// 今の標識がどのゴールのものか
	int m_flowGoalID = -1;

	bool m_isDebug = false;

	// 所有せずに索引目的のためweak_ptr
	// constの関数内で値を変えるのでmutableを付ける
	mutable std::map<int, std::weak_ptr<WayPoint>>m_wpWayPointList;
	mutable bool m_isIndexDirty = true;

private:

	WayPointManager() = default;
	~WayPointManager() = default;

public:

	static WayPointManager& Instance()
	{
		static WayPointManager instance;
		return instance;
	}

};
