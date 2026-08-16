#pragma once

class WayPoint;

class WayPointNumber
{
public:

	
	WayPointNumber(){}
	~WayPointNumber(){}

	void Init();
	void Update();
	void DrawUnLit();

	void SetTarget(std::shared_ptr<WayPoint>target) { m_wpTarget=target; }


private:

	std::weak_ptr<WayPoint>m_wpTarget;

	unsigned long    m_wayPointID = {};
	static const int maxDigits = 3;
	std::shared_ptr<KdSquarePolygon>m_spPoly[maxDigits];

	int              m_digits[maxDigits] = {}; // 各桁の数値を格納
	bool             m_numberDrawFlg[maxDigits] = {};

	Math::Vector3    m_localNumberPos[maxDigits] = {};
	
	Math::Matrix     m_mWorld[maxDigits] = {};


};
