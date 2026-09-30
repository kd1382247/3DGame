#include "Inspector.h"

#include "../EditorManager.h"


void Inspector::Draw()
{
	// 項目名や値が横に長くなった場合、はみ出た分だけ横スクロールできるようにする
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_HorizontalScrollbar;

	ImGui::Begin("Inspector",nullptr,flags);

	const auto obj = EditorManager::Instance().GetSelectedObject();

	if (!obj)
	{
		ImGui::End();
		return;
	}

	//------------------------------------------------------------
	// 上部(固定): 名前・位置・回転・大きさなど。スクロールしない
	//------------------------------------------------------------
	ImGui::Text("ObjectName:");

	ImGui::SameLine();

	// 現在選択中のオブジェクト名表示
	ImGui::Text("%s", obj->GetObjectName().c_str());

	obj->DrawInspectorHeader();

	ImGui::Separator();

	//------------------------------------------------------------
	// 中央(スクロール): パラメータなど項目が多い部分だけスクロールする
	// 下端はDeleteボタンの分だけ空けておく
	//------------------------------------------------------------
	const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

	if (ImGui::BeginChild("InspectorBody", ImVec2(0.0f, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar))
	{
		obj->DrawInspector();
	}
	ImGui::EndChild();

	//------------------------------------------------------------
	// 下部(固定): Deleteボタン。スクロールしない
	//------------------------------------------------------------
	ImGui::Separator();

	DrawDeleteButton(obj);

	ImGui::End();
}

void Inspector::DrawDeleteButton(const std::shared_ptr<KdGameObject>& obj)
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
