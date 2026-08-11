#include "WayPoint.h"

#include"../WayPointManager.h"

void WayPoint::Init()
{
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();



}

void WayPoint::DrawDebug()
{
	m_pDebugWire->AddDebugSphere(GetPos(), 0.5f, kBlueColor);
	KdGameObject::DrawDebug();
}

void WayPoint::DrawInspector()
{
	// 座標変更
	Math::Vector3 pos = GetPos();
	if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
	{
		SetPos(pos);
	}

	// 接続先一覧
	if (ImGui::Button("LinkList"))
	{
		if(!m_linkIDs.empty())
		{
			ImGui::OpenPopup("LinkListPopup");
		}
	}

	if (ImGui::BeginPopup("LinkListPopup"))
	{
		for (int id : m_linkIDs)
		{
			std::string wayPointName = "WayPoint_" + std::to_string(id);
			ImGui::Text(wayPointName.c_str());
		}
		ImGui::EndPopup();
	}


	ImGui::Text("Connection");

	// 接続関係を設定
	for (const auto& wayPoints : WayPointManager::Instance().GetWayPoints())
	{
		if (!wayPoints)
		{
			continue;
		}

		// 自分自身は表示しない
		if (wayPoints->GetID() == GetID())
		{
			continue;
		}

		bool hasLink = HasLink(wayPoints->GetID());
		if (ImGui::Checkbox(wayPoints->GetObjectName().c_str(), &hasLink))
		{
			if (hasLink)
			{
				// 双方向に接続
				WayPointManager::Instance().Connect(GetID(), wayPoints->GetID());
			}
			else
			{
				// 双方向接続解除
				WayPointManager::Instance().Disconnect(GetID(), wayPoints->GetID());
			}
		}
	}
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

	m_linkIDs.push_back(id);
}

void WayPoint::RemoveLink(int id)
{
	std::erase(m_linkIDs, id);
}

bool WayPoint::HasLink(int id) const
{
	return std::find(m_linkIDs.begin(), m_linkIDs.end(), id) != m_linkIDs.end();
}
