#include "Hierarchy.h"

#include "../EditorManager.h"
#include "../../Scene/SceneManager.h"
#include "../../../Framework/GameObject/KdGameObjectFactory.h"
#include "../../System/WayPointManager/WayPointManager.h"
#include"../../GameObject/WayPoint/WayPoint.h"

void Hierarchy::Draw()
{
	ImGui::Begin("Hierarchy");

	if (ImGui::Button("GameObject"))
	{
		m_category = HierarchyCategory::GameObject;
	}

	ImGui::SameLine();

	if (ImGui::Button("WayPoint"))
	{
		m_category = HierarchyCategory::WayPoint;
	}

	ImGui::SameLine();

	if (ImGui::Button("Stage"))
	{
		m_category = HierarchyCategory::Stage;
	}

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
	}
	ImGui::End();
}

void Hierarchy::DrawGameObjects()
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

	ImGui::Text("--------GameObjects--------");
	DrawObjectList(KdGameObject::ObjectCategory::Character);
}

void Hierarchy::DrawWayPoints()
{

	// ウェイポイントを新規作成
	if (ImGui::Button("Add WayPoint"))
	{
		auto wayPoint = WayPointManager::Instance().CreateWayPoint();

		if (wayPoint)
		{
			// CreateWayPoint()内でManagerへの登録まで完了している
			EditorManager::Instance().SetSelectedObject(wayPoint);
		}
	}

	ImGui::Text("----------WayPoints----------");

	// どのオブジェクトを選択しているか
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

	ImGui::Text("-----------Stage-----------");
	DrawObjectList(KdGameObject::ObjectCategory::Stage);

	ImGui::Text("----------Gimmicks----------");
	DrawObjectList(KdGameObject::ObjectCategory::Gimmick);
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
			EditorManager::Instance()
				.CreateGameObject(name);
		}
	}
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

