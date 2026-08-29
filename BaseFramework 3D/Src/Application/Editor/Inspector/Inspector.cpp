#include "Inspector.h"

#include "../EditorManager.h"


void Inspector::Draw()
{
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::Begin("Inspector",nullptr,flags);

	const auto obj = EditorManager::Instance().GetSelectedObject();

	if (!obj)
	{
		ImGui::End();
		return;
	}

	ImGui::Text("ObjectName:");

	ImGui::SameLine();

	// 現在選択中のオブジェクト名表示
	ImGui::Text("%s", obj->GetObjectName().c_str());

	obj->DrawInspector();

	DrawDeleteButton(obj);

	ImGui::End();
}

void Inspector::DrawDeleteButton(const std::shared_ptr<KdGameObject> obj)
{
	if (!ImGui::Button("Delete"))
	{
		return;
	}

	// 選択参照を先に解除し、Inspectorが削除対象を保持し続けないようにする
	EditorManager::Instance().SetSelectedObject(nullptr);

	
	obj->Destroy();

	EditorManager::Instance().MarkDirty();

}
