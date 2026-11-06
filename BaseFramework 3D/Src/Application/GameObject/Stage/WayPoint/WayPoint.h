#pragma once

#include"../StageOwnedObject/StageOwnedObject.h"

class StageObject;


class WayPoint : public StageOwnedObject
{
public:
	WayPoint() { Init(); }
	~WayPoint() override {}

	void Init() override;
	void PreDraw()override;
	void DrawDebug() override;

	// 固定表示(スクロールしない):位置
	void DrawInspectorHeader() override;

	// スクロール側:エリアID・接続関係
	void DrawInspector() override;

	int GetAreaID()const { return m_areaID; }
	void SetAreaID(const int areaID) { m_areaID = areaID; }


	// このWayPointから直接移動できる接続先ID
	const std::vector<int>& GetLinks() const
	{
		return m_linkIDs;
	}

	// WayPoint削除
	void Destroy()override;

	bool AddLink(int id);
	bool RemoveLink(int id);
	bool HasLink(int id) const;

private:

	// エリアID
	int m_areaID = -1;

	// 直接移動可能なWayPointのID
	std::vector<int> m_linkIDs;

};
