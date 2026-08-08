#include "EditorHierarchy.h"

#include"../EditorManager.h"
#include"../../Scene/SceneManager.h"
#include<ranges>

#include"../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../System/PathFinding/WayPointManager.h"
#include"../../System/PathFinding/WayPoint/WayPoint.h"


void EditorHierarchy::Draw()
{
	ImGui::Begin("Hierarchy");

	if (ImGui::Button("GameObject"))
	{
		m_editorObjectType = EditorObjectType::GameObject;
	}
	
	ImGui::SameLine();

	if (ImGui::Button("WayPoint"))
	{
		m_editorObjectType = EditorObjectType::WayPoint;
	}

	if (m_editorObjectType == EditorObjectType::GameObject)
	{
		DrawGameObjectHierarchy();
	}
	else if (m_editorObjectType == EditorObjectType::WayPoint)
	{
		DrawWayPointHierarchy();
	}


	ImGui::End();
}

void EditorHierarchy::DrawGameObjectHierarchy()
{
	// オブジェクトを新規作成
	if (ImGui::Button("Add Object"))
	{
		ImGui::OpenPopup("AddObjectPopup");
	}

	if (ImGui::BeginPopup("AddObjectPopup"))
	{
		const auto& objects = KdGameObjectFactory::Instance().GetCreateFunctions();
		for (const auto& name : objects | std::views::keys)
		{
			if (ImGui::MenuItem(name.c_str()))
			{
				EditorManager::Instance().CreateGameObject(name);
			}
		}

		ImGui::EndPopup();
	}
	// どのオブジェクトを選択しているか
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		ImGui::PushID(obj.get());

		if (ImGui::Selectable(obj->GetObjectName().c_str(),
			obj == EditorManager::Instance().GetSelectedObject()))
		{
			EditorManager::Instance().SetSelectedObject(obj);
		}
		ImGui::PopID();
	}
}

void EditorHierarchy::DrawWayPointHierarchy()
{

	// オブジェクトを新規作成
	if (ImGui::Button("Add WayPoint"))
	{	
		auto obj = KdGameObjectFactory::Instance().CreateGameObject("WayPoint");

		auto wayPoint = std::dynamic_pointer_cast<WayPoint>(obj);
		wayPoint->SetId(0);
		wayPoint->SetObjectName("WayPoint0");
		wayPoint->SetPos({ 0,0,0 });

		if (wayPoint)
		{
			WayPointManager::Instance().RegisterWayPoint(wayPoint);
		}
	}

	// どのオブジェクトを選択しているか
	for (auto& wayPoints : WayPointManager::Instance().GetWayPoints())
	{
		auto obj= std::dynamic_pointer_cast<KdGameObject>(wayPoints.lock());

		ImGui::PushID(obj.get());

		if (ImGui::Selectable(obj->GetObjectName().c_str(),
			obj == EditorManager::Instance().GetSelectedObject()))
		{
			EditorManager::Instance().SetSelectedObject(obj);
		}
		ImGui::PopID();
	}

}

