#include "Inspector.h"

#include "../EditorManager.h"

#include"../../GameObject/Stage/StageObject.h"
#include"../../GameObject/Gimmick/StageGimmick.h"

#include<map>


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

	// 選択中のオブジェクトごとに、ImGuiの項目ID(Position/Sizeなど)を分ける
	// (分けないと、別のオブジェクトを選んだとき、入力途中の値が新しいオブジェクトに書き込まれてしまう)
	ImGui::PushID(obj.get());

	//------------------------------------------------------------
	// 上部(固定): 名前・位置・回転・大きさなど。スクロールしない
	//------------------------------------------------------------
	ImGui::Text("ObjectName:");

	ImGui::SameLine();

	// 現在選択中のオブジェクト名表示
	ImGui::Text("%s", obj->GetObjectName().c_str());

	// コピーボタン(位置などを変更する項目の上)
	DrawCopyButton(obj);

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

	ImGui::PopID();

	ImGui::End();
}

void Inspector::DrawCopyButton(const std::shared_ptr<KdGameObject>& obj)
{
	// コピーできるのは、ギミック・AABB・OBBだけ
	if (!EditorManager::Instance().CanCopyObject(obj))
	{
		return;
	}

	if (!ImGui::Button("Copy"))
	{
		return;
	}

	auto copy = EditorManager::Instance().CopyObject(obj);

	if (!copy)
	{
		return;
	}

	// コピーしたものを選択する
	EditorManager::Instance().SetSelectedObject(copy);

	EditorManager::Instance().MarkDirty();
}

void Inspector::DrawDeleteButton(const std::shared_ptr<KdGameObject>& obj)
{
	// ステージは、ギミックなども一緒に消えるので確認してから削除する
	auto stage = std::dynamic_pointer_cast<StageObject>(obj);

	if (ImGui::Button("Delete"))
	{
		if (stage)
		{
			ImGui::OpenPopup("DeleteStageConfirm");
		}
		else
		{
			DeleteObject(obj);
		}
	}

	if (stage)
	{
		DrawDeleteStagePopup(stage);
	}
}

void Inspector::DrawDeleteStagePopup(const std::shared_ptr<StageObject>& stage)
{
	if (!ImGui::BeginPopupModal("DeleteStageConfirm", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		return;
	}

	ImGui::Text(U8("「%s」を削除します。"), stage->GetObjectName().c_str());

	// 一緒に消えるギミックを、種類ごとに数える
	std::map<std::string, int> gimmickCounts;

	for (const auto& gimmick : stage->GetOwnedGimmicks())
	{
		std::string className = gimmick->GetFactoryClassName();

		if (className.empty())
		{
			className = gimmick->GetObjectName();
		}

		++gimmickCounts[className];
	}

	if (!gimmickCounts.empty())
	{
		ImGui::Separator();

		ImGui::Text(U8("このステージのギミックも一緒に削除されます"));

		for (const auto& [name, count] : gimmickCounts)
		{
			ImGui::BulletText("%s x %d", name.c_str(), count);
		}
	}

	ImGui::Separator();

	ImGui::Text(U8("元に戻せません。"));

	if (ImGui::Button(U8("削除する"), ImVec2(120, 0)))
	{
		ImGui::CloseCurrentPopup();

		DeleteObject(stage);
	}

	ImGui::SameLine();

	if (ImGui::Button(U8("キャンセル"), ImVec2(120, 0)))
	{
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
}

void Inspector::DeleteObject(const std::shared_ptr<KdGameObject>& obj)
{
	// 選択参照を先に解除し、Inspectorが削除対象を保持し続けないようにする
	EditorManager::Instance().SetSelectedObject(nullptr);

	obj->Destroy();

	EditorManager::Instance().MarkDirty();
}
