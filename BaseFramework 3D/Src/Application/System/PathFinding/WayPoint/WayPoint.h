#pragma once

class WayPoint :public KdGameObject
{

public:

	WayPoint() { Init(); }
	~WayPoint()override{}


	void Init()override;
	void Update()override;
	void DrawDebug()override;

	int GetId()const { return m_id; }
	void SetId(int id) { m_id = id; }


	// 接続先
	const std::vector<int>& GetLinks()const
	{
		return m_linkIds;
	}

	void AddLink(int id);
	void RemoveLink(int id);
	bool HasLink(int id)const;


private:

	// WayPointを識別する番号
	int m_id = -1;

	// 直接移動可能なWayPointのID
	std::vector<int>m_linkIds;

};