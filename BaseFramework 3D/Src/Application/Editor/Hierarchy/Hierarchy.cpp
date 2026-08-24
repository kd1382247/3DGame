#include "Hierarchy.h"

#include "../EditorManager.h"
#include "../../Scene/SceneManager.h"
#include "../../../Framework/GameObject/KdGameObjectFactory.h"
#include "../../System/WayPointManager/WayPointManager.h"
#include"../../GameObject/WayPoint/WayPoint.h"
#include"../../GameObject/Stage/Stage01/Collision/WallCollision/WallCollision.h"
#include"../../GameObject/Stage/Stage01/Collision/WallCollision/WallCollisionManager.h"


void Hierarchy::Draw()
{
	ImGui::Begin("Hierarchy");

	// 表示するカテゴリを選択
	DrawCategoryButtons();

	ImGui::Separator();

	// オブジェクトを追加する
	DrawAddButtons();

	ImGui::Separator();

	// 追加されたオブジェクト一覧
	DrawScrollableList();

	ImGui::End();
}

void Hierarchy::DrawCategoryButtons()
{
	CategoryButton("GameObject", HierarchyCategory::GameObject);

	ImGui::SameLine();

	CategoryButton("Stage", HierarchyCategory::Stage);

	ImGui::SameLine();

	CategoryButton("WayPoint", HierarchyCategory::WayPoint);

	CategoryButton("CollisionBox", HierarchyCategory::CollisionBox);

}

void Hierarchy::CategoryButton(const char* label, HierarchyCategory category)
{
	const bool selected = m_category == category;

	if (selected)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.45f, 0.8f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.5f, 0.85f, 1.0f));
	}

	if (ImGui::Button(label))
	{
		m_category = category;
	}

	if (selected)
	{
		ImGui::PopStyleColor(2);
	}

}

void Hierarchy::DrawAddButtons()
{
	switch (m_category)
	{
	case Hierarchy::HierarchyCategory::GameObject:
		AddGameObject();
		break;
	case Hierarchy::HierarchyCategory::WayPoint:
		AddWayPoint();
		break;
	case Hierarchy::HierarchyCategory::Stage:
		AddStage();
		break;
	case Hierarchy::HierarchyCategory::CollisionBox:
		AddCollisionBox();
		break;
	}

}

void Hierarchy::AddGameObject()
{
	// オブジェクトを新規作成
	if (ImGui::Button("Add Object"))
	{
		ImGui::OpenPopup("AddObjectPopup");
	}

	if (ImGui::BeginPopup("AddObjectPopup"))
	{
		DrawAddObjectList(KdGameObject::ObjectCategory::Character);
		ImGui::EndPopup();
	}
}

void Hierarchy::AddWayPoint()
{
	// ウェイポイントを新規作成
	if (ImGui::Button("Add WayPoint"))
	{
		auto wayPoint = WayPointManager::Instance().CreateWayPoint();

		if (wayPoint)
		{
			// CreateWayPoint()内でManagerへの登録まで完了している
			EditorManager::Instance().SetSelectedObject(wayPoint);

			EditorManager::Instance().MarkDirty();
		}
	}

	ImGui::SameLine();

	bool isDebug = WayPointManager::Instance().IsDebug();

	if (ImGui::Checkbox("Debug", &isDebug))
	{
		isDebug ? WayPointManager::Instance().SetDebugFlg(true) :
			WayPointManager::Instance().SetDebugFlg(false);
	}
}

void Hierarchy::AddStage()
{
	// ステージを新規作成
	if (ImGui::Button("Add Stage"))
	{
		ImGui::OpenPopup("AddStagePopup");
	}

	if (ImGui::BeginPopup("AddStagePopup"))
	{
		DrawAddObjectList(KdGameObject::ObjectCategory::Stage);
		ImGui::EndPopup();
	}

	ImGui::SameLine();

	// ギミックを新規作成
	if (ImGui::Button("AddGimmick"))
	{
		ImGui::OpenPopup("AddGimmickPopup");
	}

	if (ImGui::BeginPopup("AddGimmickPopup"))
	{
		DrawAddObjectList(KdGameObject::ObjectCategory::Gimmick);
		ImGui::EndPopup();
	}

}

void Hierarchy::AddCollisionBox()
{

	if (ImGui::Button("Add WallBox"))
	{
		auto wallBox = WallCollisionManager::Instance().CreateWallCollision();

		if (wallBox)
		{
			// CreateWayPoint()内でManagerへの登録まで完了している
			EditorManager::Instance().SetSelectedObject(wallBox);

			EditorManager::Instance().MarkDirty();
		}
	}


	ImGui::SameLine();

	bool isDebug = WallCollisionManager::Instance().IsDebug();

	if (ImGui::Checkbox("Debug", &isDebug))
	{
		isDebug? WallCollisionManager::Instance().SetDebugFlg(true):
			     WallCollisionManager::Instance().SetDebugFlg(false);
	}

}

void Hierarchy::DrawGameObjects()
{
	DrawObjectList(KdGameObject::ObjectCategory::Character);
}

void Hierarchy::DrawWayPoints()
{
	for (const auto& wayPoint : WayPointManager::Instance().GetWayPoints())
	{
		if (!wayPoint)
		{
			continue;
		}

		SelectObject(wayPoint);
	}
}

void Hierarchy::DrawStage()
{
	DrawObjectList(KdGameObject::ObjectCategory::Stage);

	ImGui::Separator();

	DrawObjectList(KdGameObject::ObjectCategory::Gimmick);
}

void Hierarchy::DrawCollisionBox()
{

	for (const auto& wallBox : WallCollisionManager::Instance().GetWallCollisionList())
	{
		if (!wallBox)
		{
			continue;
		}

		SelectObject(wallBox);
	}
}

void Hierarchy::DrawAddObjectList(KdGameObject::ObjectCategory objectCategory)
{
	const auto& createFunctions =
		KdGameObjectFactory::Instance().GetCreateFunctions();

	for (const auto& [name, entry] : createFunctions)
	{
		if (entry.category != objectCategory)
		{
			continue;
		}

		if (ImGui::Selectable(
			name.c_str(),
			false,
			ImGuiSelectableFlags_DontClosePopups))
		{
			EditorManager::Instance().CreateGameObject(name);

			EditorManager::Instance().MarkDirty();
		}
	}
}

void Hierarchy::DrawScrollableList()
{
	// 残り領域をスクロール可能なChildとして使う
	if (ImGui::BeginChild(
		"HierarchyList",
		ImVec2(0, 0),
		true))
	{
		switch (m_category)
		{
		case HierarchyCategory::GameObject:
			DrawGameObjects();
			break;

		case HierarchyCategory::WayPoint:
			DrawWayPoints();
			break;

		case HierarchyCategory::Stage:
			DrawStage();
			break;

		case HierarchyCategory::CollisionBox:
			DrawCollisionBox();
			break;
		}
	}

	ImGui::EndChild();
}

void Hierarchy::DrawObjectList(KdGameObject::ObjectCategory objectCategory)
{
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		if (!obj || obj->GetObjectCategory() != objectCategory)
		{
			continue;
		}

		SelectObject(obj);
	}
}

void Hierarchy::SelectObject(const std::shared_ptr<KdGameObject>& obj)
{
	ImGui::PushID(obj.get());

	if (ImGui::Selectable(obj->GetObjectName().c_str(),
		obj == EditorManager::Instance().GetSelectedObject()))
	{
		EditorManager::Instance().SetSelectedObject(obj);
	}
	ImGui::PopID();
}

