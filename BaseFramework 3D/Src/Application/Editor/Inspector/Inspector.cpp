#include "Inspector.h"

#include "../EditorManager.h"

#include"../../GameObject/Stage/StageObject.h"
#include"../../GameObject/Gimmick/StageGimmick.h"
#include"../../GameObject/Character/CharacterBase.h"

#include"../../Scene/SceneManager.h"

#include<map>


void Inspector::OnDraw()
{
	const auto obj = EditorManager::Instance().GetSelectedObject();

	if (!obj)
	{
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

	// 親を変更できる
	DrawParentSelector(obj);

	// オブジェクトが持つ部品のInspector
	obj->DrawComponentsInspector();

	ImGui::Separator();

	//------------------------------------------------------------
	// 中央(スクロール): パラメータなど項目が多い部分だけスクロールする
	// 下端はDeleteボタンの分だけ空けておく
	//------------------------------------------------------------
	const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

	if (ImGui::BeginChild("InspectorBody", ImVec2(0.0f, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar))
	{
		// オブジェクト固有のInspector
		obj->DrawInspector();


	}
	ImGui::EndChild();

	//------------------------------------------------------------
	// 下部(固定): Deleteボタン。スクロールしない
	//------------------------------------------------------------
	ImGui::Separator();

	DrawDeleteButton(obj);

	ImGui::PopID();
}

ImGuiWindowFlags Inspector::GetImGuiWindowFlags() const
{

	// 項目名や値が横に長くなった場合、はみ出た分だけ横スクロールできるようにする
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_HorizontalScrollbar;

	return flags;
}

void Inspector::DrawCopyButton(const std::shared_ptr<KdGameObject>& obj)
{
	// ParentLinkを持つ、IsCopyableがtrueの場合コピー可能
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

	// 一緒に消えるキャラクター、ギミックを種類ごとに数える
	std::map<std::string, int> objectCounts;

	// ギミック
	for (const auto& gimmick : stage->GetOwnedGimmicks())
	{
		std::string className = gimmick->GetFactoryClassName();

		if (className.empty())
		{
			className = gimmick->GetObjectName();
		}

		++objectCounts[className];
	}

	// キャラクター
	for (const auto& character : stage->GetOwnedCharacters())
	{
		std::string className = character->GetFactoryClassName();

		if (className.empty())
		{
			className = character->GetObjectName();
		}

		++objectCounts[className];
	}

	if (!objectCounts.empty())
	{
		ImGui::Separator();

		ImGui::Text(U8("このステージのギミック、キャラクターも一緒に削除されます"));

		for (const auto& [name, count] : objectCounts)
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

void Inspector::DrawParentSelector(const std::shared_ptr<KdGameObject>& obj)
{
	// 親を変えることができない場合早期リターン
	if (!EditorManager::Instance().CanChangeParent(obj))
	{
		return;
	}

	std::vector<std::shared_ptr<StageObject>> stageObjects;

	// ステージを集める
	for (const auto& sceneObj : SceneManager::Instance().GetObjList())
	{
		auto stageObj = std::dynamic_pointer_cast<StageObject>(sceneObj);

		if (!stageObj)             { continue; }
		if (stageObj->IsExpired()) { continue; }

		stageObjects.push_back(stageObj);
	}


	auto parentObj = obj->GetComponent<ParentLink>();

	if (!parentObj)
	{
		return;
	}

	// 現在の親の名前
	std::string parentName = parentObj->GetParentName();

	// 親の名前がなかったら
	if (parentName.empty())
	{
		parentName = "None";
	}

	// コンボを書く
	if (ImGui::BeginCombo("Parent", parentName.c_str()))
	{

		for (const auto& stageObj : stageObjects)
		{
			
			ImGui::PushID(stageObj.get());

			// 現在の親の名前と、ステージの名前が違ったら
			if (ImGui::Selectable(stageObj->GetObjectName().c_str(),
				parentName == stageObj->GetObjectName()))
			{
				// 選択したステージと親のステージ名が違った場合
				if(parentName!=stageObj->GetObjectName())
				{

					parentObj->SetParentKeepWorld(stageObj, *obj);

					EditorManager::Instance().MarkDirty();
				}

			}

			ImGui::PopID();
		}


		ImGui::EndCombo();
	}
}

void Inspector::DeleteObject(const std::shared_ptr<KdGameObject>& obj)
{
	// 選択参照を先に解除し、Inspectorが削除対象を保持し続けないようにする
	EditorManager::Instance().SetSelectedObject(nullptr);

	obj->Destroy();

	EditorManager::Instance().MarkDirty();
}
