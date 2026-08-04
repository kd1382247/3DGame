#include "EditorInspector.h"

#include"../EditorManager.h"

#include"../../Scene/SceneManager.h"

void EditorInspector::Draw()
{
	ImGui::Begin("Inspector");

	auto& obj = EditorManager::Instance().GetSelectedObject();

	if (obj)
	{
		if (ImGui::Button("Duplicate"))
		{
			auto duplicate = std::make_shared<KdGameObject>();

			duplicate->SetObjectName(obj->GetObjectName() + " Copy");
			duplicate->SetPos(obj->GetPos());
			duplicate->SetScale(obj->GetScale());

			SceneManager::Instance().AddObject(duplicate);
			EditorManager::Instance().SetSelectedObject(duplicate);
		}

		ImGui::Text("ObjectName:");

		ImGui::SameLine();

		// 現在選択中のオブジェクト名表示
		ImGui::Text(obj->GetObjectName().c_str());

		obj->DrawInspecter();

		Delete(obj);
	}

	ImGui::End();
}

void EditorInspector::SaveParameter(const std::shared_ptr<KdGameObject>& obj)
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
