#include "WayPoint.h"

#include"../../Editor/EditorManager.h"

#include"../Gimmick/AreaVolume/AreaVolume.h"

#include "../../System/WayPointManager/WayPointManager.h"

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

void WayPoint::DrawDebug()
{
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

	// IDを描画
	//DrawID();
}

void WayPoint::DrawInspectorHeader()
{
	// 座標変更(Stageからのローカル位置)
	Math::Vector3 pos = GetLocalPos();

	if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
	{
		SetLocalPos(pos);
		EditorManager::Instance().MarkDirty();
	}
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

				if (wayPoint->GetOwner() != GetOwner())
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

void WayPoint::SetID(int id)
{
	m_id = id;
}

void WayPoint::SetStageTransform(const Math::Vector3& stagePos, const Math::Vector3& stageScale)
{
	SetPos(stagePos + m_localPos * stageScale);
}

void WayPoint::Destroy()
{
	WayPointManager::Instance().RemoveWayPoint(GetID());
}

bool WayPoint::AddLink(int id)
{
	// 自分自身には接続しない
	if (id == m_id)
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
