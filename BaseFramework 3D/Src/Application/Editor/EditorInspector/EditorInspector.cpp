#include "EditorInspector.h"

#include"../EditorManager.h"

#include"../../Scene/SceneManager.h"

void EditorInspector::Draw()
{
	ImGui::Begin("Inspector");

	auto& obj = EditorManager::Instance().GetSelectedObject();

	if (!obj)
	{
		ImGui::End();
		return;
	}

	ImGui::Text("ObjectName:");

	ImGui::SameLine();

	// 現在選択中のオブジェクト名表示
	ImGui::Text(obj->GetObjectName().c_str());

	obj->DrawInspecter();

	Delete(obj);
	

	ImGui::End();
}

void EditorInspector::DrawGameObjectInspecter()
{

}

void EditorInspector::DrawWayPointInspecter()
{
	
}

void EditorInspector::Delete(const std::shared_ptr<KdGameObject>& obj)
{
	// オブジェクト削除
	if (ImGui::Button("Delete"))
	{
		obj->Destroy();
		EditorManager::Instance().SetSelectedObject(nullptr);
	}
}
