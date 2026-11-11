#include "WayPoint.h"

#include"../../../Editor/EditorManager.h"

#include"../../Gimmick/AreaVolume/AreaVolume.h"

#include "WayPointManager.h"

void WayPoint::Init()
{
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// カテゴリーをセット
	SetObjectCategory(ObjectCategory::None);

	// 当たり判定をセット
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	("WayPoint", Math::Vector3(0, 0, 0), 0.5, KdCollider::TypeBump);

}

void WayPoint::PreDraw()
{
	StageOwnedObject::PreDraw();

	int AreaID = -1;

	// エリアIDを決める
	for (const auto& weakArea : WayPointManager::Instance().GetAreaVolumes())
	{
		auto spArea = weakArea.lock();

		if (!spArea) { continue; }

		// エリア内に入っていたら、そのIDを入れる
		if (spArea->IsInside(GetPos()+Math::Vector3(0.0f,0.5f,0.0f)))
		{
			AreaID = spArea->GetAreaID();
			break;
		}
	}

	SetAreaID(AreaID);

	WayPointManager::Instance().InvalidateIndex();
	WayPointManager::Instance().InvalidateFlowField();

}

void WayPoint::DrawDebug()
{

	if (!WayPointManager::Instance().IsDebug())
	{
		return;
	}

	if (GetParent() != EditorManager::Instance().GetActiveStage())
	{
		return;
	}

	// 線の表示
	for (int linkID : GetLinks())
	{
		auto linkedPoint = WayPointManager::Instance().FindWayPoint(linkID);

		if (!linkedPoint)
		{
			continue;
		}

		// 小さいID側だけ登録
		if (GetID() > linkID)
		{
			continue;
		}

		m_pDebugWire->AddDebugLine(GetPos(), linkedPoint->GetPos(), kGreenColor);
	}



	Math::Color color;

	// 球の表示
	if(IsSelected())
	{
		color = kRedColor+kGreenColor;
	}
	else
	{
		// 通常はエリアIDごとの色(キャラクター・AreaVolumeと同じ色)
		color = AreaVolume::GetAreaColor(GetAreaID());
	}

	m_pDebugWire->AddDebugSphere(GetPos(), 0.5f, color);
	KdGameObject::DrawDebug();
}

void WayPoint::DrawInspector()
{
	// 接続先一覧
	if (ImGui::Button("LinkList"))
	{
		if (!m_linkIDs.empty())
		{
			ImGui::OpenPopup("LinkListPopup");
		}
	}

	if (ImGui::BeginPopup("LinkListPopup"))
	{
		for (int id : m_linkIDs)
		{
			const std::string wayPointName =
				"WayPoint_" + std::to_string(id);

			ImGui::Text("%s", wayPointName.c_str());
		}
		ImGui::EndPopup();
	}

	if (ImGui::CollapsingHeader("Connections",ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 残り領域をスクロール可能なChildとして使う
		if (ImGui::BeginChild("HierarchyList", ImVec2(0, 200), true))
		{
			// 接続関係を設定
			for (const auto& wayPoint : WayPointManager::Instance().GetWayPoints())
			{
				if (!wayPoint)
				{
					continue;
				}

				// 自分自身は表示しない
				if (wayPoint->GetID() == GetID())
				{
					continue;
				}

				if (wayPoint->GetParent() != GetParent())
				{
					continue;
				}

				bool hasLink = HasLink(wayPoint->GetID());

				// 同名WayPointがあってもImGui上の項目を区別できるようIDを付ける
				ImGui::PushID(wayPoint->GetID());

				if (ImGui::Checkbox(wayPoint->GetObjectName().c_str(), &hasLink))
				{
					if (hasLink)
					{
						// 双方向に接続
						WayPointManager::Instance().Connect(GetID(), wayPoint->GetID());
						EditorManager::Instance().MarkDirty();
					}
					else
					{
						// 双方向接続解除
						WayPointManager::Instance().Disconnect(GetID(), wayPoint->GetID());
						EditorManager::Instance().MarkDirty();
					}
				}

				ImGui::PopID();
			}
		}

		ImGui::EndChild();
	}
}

void WayPoint::Destroy()
{
	// 接続しているウェイポイントを解除する
	WayPointManager::Instance().UnLinkID(GetID());

	KdGameObject::Destroy();
}

bool WayPoint::AddLink(int id)
{
	// 自分自身には接続しない
	if (id == GetID())
	{
		return false;
	}

	// 同じIDを重複登録しない
	if (HasLink(id))
	{
		return false;
	}

	m_linkIDs.push_back(id);

	return true;
}

bool WayPoint::RemoveLink(int id)
{
	return std::erase(m_linkIDs, id) > 0;
}

bool WayPoint::HasLink(int id) const
{
	return std::find(m_linkIDs.begin(), m_linkIDs.end(), id) != m_linkIDs.end();
}