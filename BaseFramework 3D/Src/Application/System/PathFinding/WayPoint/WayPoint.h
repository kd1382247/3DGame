#pragma once

class WayPoint :public KdGameObject
{

public:

	WayPoint() { Init(); }
	~WayPoint()override{}


	void Init()override;
	void DrawDebug()override;

	void DrawInspector()override;

	int GetID()const { return m_id; }
	void SetID(int id) { m_id = id; }

	// 接続先
	const std::vector<int>& GetLinks()const
	{
		return m_linkIDs;
	}

	void AddLink(int id);
	void RemoveLink(int id);
	bool HasLink(int id)const;


private:

	// WayPointを識別する番号
	int m_id = -1;

	// 直接移動可能なWayPointのID
	std::vector<int>m_linkIDs;



};