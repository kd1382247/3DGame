#pragma once

class WayPoint : public KdGameObject
{
public:
	WayPoint() { Init(); }
	~WayPoint() override = default;

	void Init() override;
	void DrawDebug() override;

	void DrawInspector() override;

	int GetID() const { return m_id; }
	void SetID(int id) { m_id = id; }

	// このWayPointから直接移動できる接続先ID
	const std::vector<int>& GetLinks() const
	{
		return m_linkIDs;
	}

	bool AddLink(int id);
	bool RemoveLink(int id);
	bool HasLink(int id) const;

private:
	// WayPointManager内でWayPointを識別する一意な番号
	int m_id = -1;

	// 直接移動可能なWayPointのID
	std::vector<int> m_linkIDs;
};
