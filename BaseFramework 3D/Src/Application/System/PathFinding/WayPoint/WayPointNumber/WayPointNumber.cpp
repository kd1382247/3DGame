#include "WayPointNumber.h"

void WayPointNumber::Init()
{
	if (!m_spPoly)
	{
		m_spPoly = std::make_shared<KdSquarePolygon>();
		m_spPoly->SetMaterial("Asset/Textures/WayPoint/Numbers.png");





	}
}

void WayPointNumber::Update()
{

}

void WayPointNumber::DrawUnLit()
{

}
