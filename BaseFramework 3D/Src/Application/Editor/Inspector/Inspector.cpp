#include "Inspector.h"

#include "../EditorManager.h"
#include "../../System/PathFinding/WayPointManager.h"
#include "../../System/PathFinding/WayPoint/WayPoint.h"

void Inspector::Draw()
{
	ImGui::Begin("Inspector");

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

	DrawDeleteButton(obj);

	obj->DrawInspector();

	
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

	if (const auto wayPoint = std::dynamic_pointer_cast<WayPoint>(obj))
	{
		WayPointManager::Instance().RemoveWayPoint(wayPoint->GetID());
		return;
	}

	obj->Destroy();
}
