#pragma once

class WayPointNumber
{
public:

	
	WayPointNumber(){}
	~WayPointNumber(){}

	void Init();
	void Update();
	void DrawUnLit();


private:

	std::shared_ptr<KdSquarePolygon>m_spPoly = nullptr;

};
