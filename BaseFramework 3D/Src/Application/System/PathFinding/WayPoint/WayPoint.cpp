#include "WayPoint.h"

void WayPoint::Init()
{
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

}

void WayPoint::Update()
{

}

void WayPoint::DrawDebug()
{

	m_pDebugWire->AddDebugSphere(GetPos(), 0.5f, kBlueColor);

	KdGameObject::DrawDebug();
}

void WayPoint::AddLink(int id)
{
	// 自分自身には接続しない
	if (id == m_id)
	{
		return;
	}

	// 同じIDを重複登録しない
	if (HasLink(id))
	{
		return;
	}

	m_linkIds.push_back(id);
}

void WayPoint::RemoveLink(int id)
{
	std::erase(m_linkIds, id);
}

bool WayPoint::HasLink(int id) const
{
	return std::find(m_linkIds.begin(), m_linkIds.end(), id) != m_linkIds.end();
}
