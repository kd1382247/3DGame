#include "EditorHierarchy.h"

#include"../EditorManager.h"
#include"../../Scene/SceneManager.h"

void EditorHierarchy::Draw()
{
	ImGui::Begin("Hierarchy");

	// オブジェクトを新規作成
	if (ImGui::Button("Create Enpty"))
	{
		auto newObject = std::make_shared<KdGameObject>();
		newObject->SetObjectName("GameObject");

		SceneManager::Instance().AddObject(newObject);
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
