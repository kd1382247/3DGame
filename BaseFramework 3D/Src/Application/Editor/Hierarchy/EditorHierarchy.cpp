#include "EditorHierarchy.h"

#include"../EditorManager.h"
#include"../../Scene/SceneManager.h"

void EditorHierarchy::Draw()
{
	ImGui::Begin("Hierarchy");

	// オブジェクトを新規作成
	if (ImGui::Button("Add Object"))
	{
		ImGui::OpenPopup("AddObjectPopup");
	}

	if (ImGui::BeginPopup("AddObjectPopup"))
	{
		if (ImGui::MenuItem("Create Empty"))
		{
			EditorManager::Instance()
				.CreateObject("KdGameObject");
		}

		if (ImGui::MenuItem("Player"))
		{
			EditorManager::Instance()
				.CreateObject("Player");
		}

		if (ImGui::MenuItem("Ground"))
		{
			EditorManager::Instance()
				.CreateObject("Ground");
		}

		if (ImGui::MenuItem("TPS Camera"))
		{
			EditorManager::Instance()
				.CreateObject("TPSCamera");
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

	ImGui::End();
}
