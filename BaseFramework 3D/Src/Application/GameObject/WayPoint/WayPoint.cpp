#include "WayPoint.h"

#include"../../Editor/EditorManager.h"

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

	// IDの表示をするポリゴンを初期化
	for (int i = 0; i < maxDigits; ++i)
	{
		if (!m_spPoly[i])
		{
			m_spPoly[i] = std::make_shared<KdSquarePolygon>();
			m_spPoly[i]->SetMaterial("Asset/Textures/Numbers/Numbers.png");

			m_spPoly[i]->SetSplit(10, 1);

			m_spPoly[i]->SetScale(0.5);
		}
	}
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
		color = kGreenColor;
	}

	m_pDebugWire->AddDebugSphere(GetPos(), 0.5f, color);
	KdGameObject::DrawDebug();

	// IDを描画
	//DrawID();
}

void WayPoint::DrawInspector()
{

	DrawPositionInspector();


	// AreaIDをセット

	int areaID = GetAreaID();
	int min = 0;
	int max = 2;


	if (ImGui::DragInt("Area ID", &areaID, 1.0f))
	{
		areaID=std::clamp(areaID, min, max);
		SetAreaID(areaID);
	}



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

	SetUpDrawID();
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

void WayPoint::DrawID()
{
	
	for (int i = 0; i < m_renderDigitCount; i++)
	{

		Math::Matrix mat = Math::Matrix::CreateTranslation(
			m_localNumberPos[i]) * m_mWorld;

		KdShaderManager::Instance().m_StandardShader.DrawPolygon(
			*m_spPoly[i], mat);

	}
}

void WayPoint::SetUpDrawID()
{
	// ウェイポイント数を表示
	// 配列に各桁の数値を格納

	m_wayPointID = GetID();
	int tmp = m_wayPointID;
	
	for (int i = 0; i < maxDigits; ++i)
	{
		// 下位の桁から抽出し、配列に格納
		m_digits[i] = tmp % 10;
		tmp /= 10;
	}

	// 最低でも1桁表示
	m_renderDigitCount = 1;

	// 必要な桁数を調べる
	for (int i = maxDigits-1; i >= 1; --i)
	{
		if (m_digits[i] != 0)
		{
			m_renderDigitCount=i+1;
			break;
		}
	}

	// 表示する数字を設定
	for (int i = 0; i < m_renderDigitCount; ++i)
	{
		m_spPoly[i]->SetUVRect(m_digits[i]);
	}

	// 桁数に応じて描画する位置をセット

	float digitSpacing = 0.4f;

	float startX =-((m_renderDigitCount - 1) * digitSpacing) * 0.5f;

	for (int i = 0; i < m_renderDigitCount; ++i)
	{
		Math::Vector3 pos =
		{
			startX + i * digitSpacing,
			0.0f,
			0.0f
		};

		m_localNumberPos[i]=-pos;
	}


}
