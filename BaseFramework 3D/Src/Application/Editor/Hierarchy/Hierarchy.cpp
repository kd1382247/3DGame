#include "Hierarchy.h"

#include"../EditorManager.h"
#include"../../Scene/SceneManager.h"
#include<ranges>

#include"../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../System/PathFinding/WayPointManager.h"
#include"../../System/PathFinding/WayPoint/WayPoint.h"


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
	case Hierarchy::HierarchyCategory::GameObject:
		DrawGameObjects();
		break;
	case Hierarchy::HierarchyCategory::WayPoint:
		DrawWayPoints();
		break;
	case Hierarchy::HierarchyCategory::Stage:
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
		const auto& createFanctions = KdGameObjectFactory::Instance().GetCreateFunctions();
		AddObject(createFanctions, KdGameObject::ObjectCategory::Character);

		ImGui::EndPopup();
	}


	ImGui::Text("--------GameObjects--------");

	// どのオブジェクトを選択しているか
	for (auto& obj : SceneManager::Instance().GetObjList())
	{

		if (!obj)
		{
			continue;
		}
		// 敵、プレイヤーのみを表示
		if (obj->GetObjectCategory() == KdGameObject::ObjectCategory::Character)
		{
			SelectObject(obj);
		}
	}

}

void Hierarchy::DrawWayPoints()
{

	// ウェイポイントを新規作成
	if (ImGui::Button("Add WayPoint"))
	{	
		auto wayPoint = WayPointManager::Instance().CreateWayPoint();

		if (wayPoint)
		{
			WayPointManager::Instance().RegisterWayPoint(wayPoint);
			EditorManager::Instance().SetSelectedObject(wayPoint);
		}
	}

	ImGui::Text("----------WayPoints----------");

	// どのオブジェクトを選択しているか
	for (auto& wayPoint : WayPointManager::Instance().GetWayPoints())
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
		const auto& createFanctions = KdGameObjectFactory::Instance().GetCreateFunctions();
		// オブジェクト生成
		AddObject(createFanctions, KdGameObject::ObjectCategory::Stage);

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
		const auto& createFanctions = KdGameObjectFactory::Instance().GetCreateFunctions();
		// オブジェクト生成
		AddObject(createFanctions, KdGameObject::ObjectCategory::Gimmick);

		ImGui::EndPopup();
	}

	ImGui::Text("-----------Stage-----------");

	// どのオブジェクトを選択しているか
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (!obj)
		{
			continue;
		}
		if (obj->GetObjectCategory() == KdGameObject::ObjectCategory::Stage)
		{
			SelectObject(obj);
		}
	}

	ImGui::Text("----------Gimmicks----------");
	// どのオブジェクトを選択しているか
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		if (!obj)
		{
			continue;
		}
		if (obj->GetObjectCategory() == KdGameObject::ObjectCategory::Gimmick)
		{
			SelectObject(obj);
		}
	}
}

void Hierarchy::AddObject(const auto& createFanctions, const KdGameObject::ObjectCategory objectCategory)
{
	for (const auto& [name, entory] : createFanctions)
	{
		if (entory.category !=objectCategory )
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

void Hierarchy::SelectObject(const std::shared_ptr<KdGameObject>&obj)
{
	ImGui::PushID(obj.get());

	if (ImGui::Selectable(obj->GetObjectName().c_str(),
		obj == EditorManager::Instance().GetSelectedObject()))
	{
		EditorManager::Instance().SetSelectedObject(obj);
	}
	ImGui::PopID();
}

