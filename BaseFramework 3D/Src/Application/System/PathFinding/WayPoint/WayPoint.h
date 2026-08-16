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
	void SetID(int id);


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

	void DrawID();
	void SetUpDrawID();


	// WayPointManager内でWayPointを識別する一意な番号
	int m_id = -1;

	// 直接移動可能なWayPointのID
	std::vector<int> m_linkIDs;

	// ウェイポイントのIDを描画する
	unsigned long    m_wayPointID = {};
	static const int maxDigits = 3;
	std::shared_ptr<KdSquarePolygon>m_spPoly[maxDigits] = {};

	int              m_digits[maxDigits] = {}; // 各桁の数値を格納

	int              m_renderDigitCount = {};


	bool             m_digitDrawFlg[maxDigits] = {};

	Math::Vector3    m_localNumberPos[maxDigits] = {};
};
