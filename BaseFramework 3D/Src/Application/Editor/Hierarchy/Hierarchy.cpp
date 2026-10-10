#include "Hierarchy.h"

#include "../EditorManager.h"
#include "../../Scene/SceneManager.h"
#include "../../../Framework/GameObject/KdGameObjectFactory.h"
#include "../../System/WayPointManager/WayPointManager.h"
#include"../../GameObject/WayPoint/WayPoint.h"
#include"../../GameObject/Stage/Collision/AABBCollision/AABBCollision.h"
#include"../../GameObject/Stage/Collision/AABBCollision/AABBCollisionManager.h"

#include"../../GameObject/Stage/Collision/OBBCollision/OBBCollision.h"
#include"../../GameObject/Stage/Collision/OBBCollision/OBBCollisionManager.h"

#include"../../GameObject/Stage/StageObject.h"

#include<algorithm>
#include<cctype>

void Hierarchy::Draw()
{

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::Begin("Hierarchy",nullptr,flags);

	// カテゴリ選択(ボタンを押すと一覧が出て選択でき、選んだカテゴリ名をボタンの隣に表示する)
	DrawCategorySelector();


	const bool isStageDataCategory =
		m_category == HierarchyCategory::AABB ||
		m_category == HierarchyCategory::OBB ||
		m_category == HierarchyCategory::WayPoint;

	if (isStageDataCategory)
	{
		ImGui::Separator();
		DrawActiveStageSelector();

		// 同じモデルの2つ目以降のステージは、データを編集できない
		if (!EditorManager::Instance().CanEditStageData())
		{
			auto activeStage = EditorManager::Instance().GetActiveStage();

			auto primaryStage = EditorManager::Instance().FindPrimaryStage(
				activeStage->GetStageModelName());

			ImGui::Separator();
			ImGui::TextWrapped(
				U8("同じモデルは「%s」でだけ編集できます"),
				primaryStage ? primaryStage->GetObjectName().c_str() : "");

			// Beginに対応するEndを呼んでから抜ける
			ImGui::End();
			return;
		}
	}

	ImGui::Separator();

	// 選択中カテゴリのAddボタン
	DrawAddButtons();

	ImGui::Separator();

	// 検索ボックス(名前の一部で一覧を絞り込む)
	DrawSearchFilter();

	// 選択中カテゴリのオブジェクト一覧
	// (名前が長いオブジェクトがあってもはみ出た分だけ横スクロールできるようにする)
	if (ImGui::BeginChild(
		"HierarchyList",
		ImVec2(0, 0),
		true,
		ImGuiWindowFlags_HorizontalScrollbar))
	{
		DrawSelectedCategoryList();
	}


	ImGui::EndChild();

	ImGui::End();
}

void Hierarchy::DrawCategorySelector()
{
	if (ImGui::Button("Category"))
	{
		ImGui::OpenPopup("CategorySelectPopup");
	}

	if (ImGui::BeginPopup("CategorySelectPopup"))
	{
		CategorySelectItem("GameObject", HierarchyCategory::GameObject);
		CategorySelectItem("Stage",      HierarchyCategory::Stage);
		CategorySelectItem("Gimmick",    HierarchyCategory::Gimmick);
		CategorySelectItem("WayPoint",   HierarchyCategory::WayPoint);
		CategorySelectItem("AABB",       HierarchyCategory::AABB);
		CategorySelectItem("OBB",        HierarchyCategory::OBB);

		ImGui::EndPopup();
	}

	ImGui::SameLine();

	// 現在選択中のカテゴリ名をボタンの隣に表示
	// (EditorManager::UpdateMouseSelection()もこのm_categoryを見て、
	//  シーンビュークリック時にどのカテゴリのオブジェクトを選択対象にするか決めている)
	ImGui::Text("%s", GetCategoryLabel(m_category));
}

void Hierarchy::CategorySelectItem(const char* label, HierarchyCategory category)
{
	// Selectableはデフォルトでクリック時に親のポップアップを閉じてくれる
	if (ImGui::Selectable(label, m_category == category))
	{
		m_category = category;

		// 別カテゴリに切り替えたとき、検索結果が0件のまま気づきにくいので検索欄はクリアする
		m_searchFilter[0] = '\0';
	}
}

const char* Hierarchy::GetCategoryLabel(HierarchyCategory category)
{
	switch (category)
	{
	case HierarchyCategory::GameObject:   return "GameObject";
	case HierarchyCategory::Stage:        return "Stage";
	case HierarchyCategory::Gimmick:      return "Gimmick";
	case HierarchyCategory::WayPoint:     return "WayPoint";
	case HierarchyCategory::AABB:         return "AABB";
	case HierarchyCategory::OBB:          return "OBB";
	}

	return "";
}

void Hierarchy::DrawActiveStageSelector()
{
	auto activeStage = EditorManager::Instance().GetActiveStage();

	if (!activeStage)
	{
		for (const auto& obj : SceneManager::Instance().GetObjList())
		{
			auto stage= std::dynamic_pointer_cast<StageObject>(obj);
			if (stage)
			{
				activeStage = stage;
				EditorManager::Instance().SetActiveStage(stage);

				break;
			}
		}
	}

	std::string stageName = {};

	if (activeStage)
	{
		stageName = activeStage->GetObjectName();
	}
	else
	{
		stageName = "None";
	}

	if (ImGui::BeginCombo("Active Stage", stageName.c_str()))
	{

		for (const auto& obj : SceneManager::Instance().GetObjList())
		{
			auto stage = std::dynamic_pointer_cast<StageObject>(obj);
			if (stage)
			{
				ImGui::PushID(stage.get());

				if (ImGui::Selectable(stage->GetObjectName().c_str(),stage == activeStage))
				{
					EditorManager::Instance().SetActiveStage(stage);
					EditorManager::Instance().SetSelectedObject(nullptr);

					// デバッグ表示フラグをfalseにする
					WayPointManager::Instance().SetIsDebug(false);
					AABBCollisionManager::Instance().SetIsDebug(false);
					OBBCollisionManager::Instance().SetIsDebug(false);
				}
				ImGui::PopID();
			}
		}

		ImGui::EndCombo();
	}

}

void Hierarchy::DrawAddButtons()
{
	switch (m_category)
	{
	case HierarchyCategory::GameObject:
		AddGameObject();
		break;
	case HierarchyCategory::WayPoint:
		AddWayPoint();
		break;
	case HierarchyCategory::Stage:
		AddStage();
		break;
	case HierarchyCategory::Gimmick:
		AddGimmick();
		break;
	case HierarchyCategory::AABB:
		AddAABB();
		break;
	case HierarchyCategory::OBB:
		AddOBB();
		break;
	}
}

void Hierarchy::AddGameObject()
{
	// オブジェクトを新規作成
	if (ImGui::Button("Add Object"))
	{
		ImGui::OpenPopup("AddObjectPopup");
	}

	if (ImGui::BeginPopup("AddObjectPopup"))
	{
		DrawAddObjectList(KdGameObject::ObjectCategory::Character);
		ImGui::EndPopup();
	}
}

void Hierarchy::AddWayPoint()
{
	// ウェイポイントを新規作成
	if (ImGui::Button("Add WayPoint"))
	{
		auto wayPoint = WayPointManager::Instance().CreateWayPoint();

		if (wayPoint)
		{

			wayPoint->SetOwner(EditorManager::Instance().GetActiveStage());

			// CreateWayPoint()内でManagerへの登録まで完了している
			EditorManager::Instance().SetSelectedObject(wayPoint);

			EditorManager::Instance().MarkDirty();
		}
	}

	ImGui::SameLine();

	bool isDebug = WayPointManager::Instance().IsDebug();

	if (ImGui::Checkbox("Debug", &isDebug))
	{
		isDebug ? WayPointManager::Instance().SetIsDebug(true) :
			WayPointManager::Instance().SetIsDebug(false);
	}
}

void Hierarchy::AddStage()
{
	// ステージを新規作成
	if (ImGui::Button("Add Stage"))
	{
		ImGui::OpenPopup("AddStagePopup");
	}

	if (ImGui::BeginPopup("AddStagePopup"))
	{
		DrawAddObjectList(KdGameObject::ObjectCategory::Stage);
		ImGui::EndPopup();
	}
}

void Hierarchy::AddGimmick()
{
	// ギミックを新規作成
	if (ImGui::Button("Add Gimmick"))
	{
		ImGui::OpenPopup("AddGimmickPopup");
	}

	if (ImGui::BeginPopup("AddGimmickPopup"))
	{
		DrawAddObjectList(KdGameObject::ObjectCategory::Gimmick);
		ImGui::EndPopup();
	}
}

void Hierarchy::AddAABB()
{

	if (ImGui::Button("Add AABB"))
	{
		auto aabbBox = AABBCollisionManager::Instance().CreateAABBCollision();

		if (aabbBox)
		{

			aabbBox->SetOwner(EditorManager::Instance().GetActiveStage());

			// CreateWayPoint()内でManagerへの登録まで完了している
			EditorManager::Instance().SetSelectedObject(aabbBox);

			EditorManager::Instance().MarkDirty();
		}
	}


	ImGui::SameLine();

	bool isDebug = AABBCollisionManager::Instance().IsDebug();

	if (ImGui::Checkbox("Debug", &isDebug))
	{
		isDebug? AABBCollisionManager::Instance().SetIsDebug(true):
			     AABBCollisionManager::Instance().SetIsDebug(false);
	}

}

void Hierarchy::AddOBB()
{
	if (ImGui::Button("Add OBB"))
	{
		auto obb = OBBCollisionManager::Instance().CreateOBBCollision();
		
		if (obb)
		{
			obb->SetOwner(EditorManager::Instance().GetActiveStage());

			// CreateWayPoint()内でManagerへの登録まで完了している
			EditorManager::Instance().SetSelectedObject(obb);

			EditorManager::Instance().MarkDirty();
		}
	}

	ImGui::SameLine();

	bool isDebug = OBBCollisionManager::Instance().IsDebug();

	if (ImGui::Checkbox("Debug", &isDebug))
	{
		isDebug ? OBBCollisionManager::Instance().SetIsDebug(true) :
			OBBCollisionManager::Instance().SetIsDebug(false);
	}
}

void Hierarchy::DrawSearchFilter()
{
	ImGui::SetNextItemWidth(-1);// ウィンドウの横幅いっぱいに広げる

	ImGui::InputTextWithHint(
		"##HierarchySearchFilter",
		U8("検索..."),
		m_searchFilter,
		sizeof(m_searchFilter));
}

bool Hierarchy::MatchesSearchFilter(const std::string& name) const
{
	if (m_searchFilter[0] == '\0')
	{
		return true;
	}

	// 大文字小文字を区別せずに部分一致で判定する
	std::string lowerName = name;
	std::string lowerFilter = m_searchFilter;

	auto toLower = [](unsigned char c) { return static_cast<char>(std::tolower(c)); };

	std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), toLower);
	std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(), toLower);

	return lowerName.find(lowerFilter) != std::string::npos;
}

void Hierarchy::DrawSelectedCategoryList()
{
	switch (m_category)
	{
	case HierarchyCategory::GameObject:
		DrawGameObjects();
		break;

	case HierarchyCategory::WayPoint:
		DrawWayPoints();
		break;

	case HierarchyCategory::Stage:
		DrawStage();
		break;

	case HierarchyCategory::Gimmick:
		DrawGimmick();
		break;

	case HierarchyCategory::AABB:
		DrawAABB();
		break;

	case HierarchyCategory::OBB:
		DrawOBB();
		break;
	}
}

void Hierarchy::DrawGameObjects()
{
	DrawObjectList(KdGameObject::ObjectCategory::Character);
}

void Hierarchy::DrawWayPoints()
{

	auto activeStage = EditorManager::Instance().GetActiveStage();

	for (const auto& wayPoint : WayPointManager::Instance().GetWayPoints())
	{
		if (!wayPoint ||
			wayPoint->GetOwner() != activeStage)
		{
			continue;
		}

		SelectHierarchyObject(wayPoint);
	}
}

void Hierarchy::DrawStage()
{
	DrawObjectList(KdGameObject::ObjectCategory::Stage);

	// 選択中のオブジェクトがステージ(部屋)なら、
	   // その部屋を「編集するステージ」にする
	auto selectedStage = std::dynamic_pointer_cast<StageObject>(
		EditorManager::Instance().GetSelectedObject());

	if (selectedStage)
	{
		EditorManager::Instance().SetActiveStage(selectedStage);
	}

}

void Hierarchy::DrawGimmick()
{

	DrawObjectList(KdGameObject::ObjectCategory::Gimmick);
}

void Hierarchy::DrawAABB()
{

	auto activeStage = EditorManager::Instance().GetActiveStage();

	for (const auto& aabbBox : AABBCollisionManager::Instance().GetAABBCollisionList())
	{
		if (!aabbBox||
			aabbBox->GetOwner() != activeStage)
		{
			continue;
		}

		SelectHierarchyObject(aabbBox);
	}
}

void Hierarchy::DrawOBB()
{

	auto activeStage = EditorManager::Instance().GetActiveStage();

	for (const auto& obb : OBBCollisionManager::Instance().GetOBBCollisionList())
	{
		if (!obb||
			obb->GetOwner()!=activeStage)
		{
			continue;
		}

		SelectHierarchyObject(obb);
	}
}

void Hierarchy::DrawAddObjectList(KdGameObject::ObjectCategory objectCategory)
{
	const auto& createFunctions =
		KdGameObjectFactory::Instance().GetCreateFunctions();

	for (const auto& [name, entry] : createFunctions)
	{
		if (entry.category != objectCategory)
		{
			continue;
		}

		if (ImGui::Selectable(
			name.c_str(),
			false,
			ImGuiSelectableFlags_DontClosePopups))
		{
			EditorManager::Instance().CreateGameObject(name);

			EditorManager::Instance().MarkDirty();
		}
	}
}

void Hierarchy::DrawObjectList(KdGameObject::ObjectCategory objectCategory)
{
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		if (!obj || obj->GetObjectCategory() != objectCategory)
		{
			continue;
		}

		SelectHierarchyObject(obj);
	}
}

void Hierarchy::SelectHierarchyObject(const std::shared_ptr<KdGameObject>& obj)
{
	const std::string& name = obj->GetObjectName();

	// 検索ボックスに一致しないオブジェクトは表示しない
	if (!MatchesSearchFilter(name))
	{
		return;
	}

	ImGui::PushID(obj.get());

	// 名前がリストの表示幅より長い場合は、テキスト幅ぴったりのサイズにする
	// (幅を0のままにすると表示領域に合わせて縮められてしまい、横スクロールが発生しない)
	const float textWidth = ImGui::CalcTextSize(name.c_str()).x;
	const float availWidth = ImGui::GetContentRegionAvail().x;
	const float selectableWidth = (textWidth > availWidth) ? textWidth : 0.0f;

	if (ImGui::Selectable(name.c_str(),
		obj == EditorManager::Instance().GetSelectedObject(),
		0,
		ImVec2(selectableWidth, 0)))
	{
		EditorManager::Instance().SetSelectedObject(obj);
	}
	ImGui::PopID();
}
