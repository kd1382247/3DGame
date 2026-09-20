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
		CategorySelectItem("Stage", HierarchyCategory::Stage);
		CategorySelectItem("WayPoint", HierarchyCategory::WayPoint);
		CategorySelectItem("CollisionBox", HierarchyCategory::CollisionBox);
		CategorySelectItem("OBB", HierarchyCategory::OBB);

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
	case HierarchyCategory::WayPoint:     return "WayPoint";
	case HierarchyCategory::CollisionBox: return "CollisionBox";
	case HierarchyCategory::OBB:          return "OBB";
	}

	return "";
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
	case HierarchyCategory::CollisionBox:
		AddCollisionBox();
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
			// CreateWayPoint()内でManagerへの登録まで完了している
			EditorManager::Instance().SetSelectedObject(wayPoint);

			EditorManager::Instance().MarkDirty();
		}
	}

	ImGui::SameLine();

	bool isDebug = WayPointManager::Instance().IsDebug();

	if (ImGui::Checkbox("Debug", &isDebug))
	{
		isDebug ? WayPointManager::Instance().SetDebugFlg(true) :
			WayPointManager::Instance().SetDebugFlg(false);
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

	ImGui::SameLine();

	// ギミックを新規作成
	if (ImGui::Button("AddGimmick"))
	{
		ImGui::OpenPopup("AddGimmickPopup");
	}

	if (ImGui::BeginPopup("AddGimmickPopup"))
	{
		DrawAddObjectList(KdGameObject::ObjectCategory::Gimmick);
		ImGui::EndPopup();
	}

}

void Hierarchy::AddCollisionBox()
{

	if (ImGui::Button("Add AABBBox"))
	{
		auto aabbBox = AABBCollisionManager::Instance().CreateAABBCollision();

		if (aabbBox)
		{
			// CreateWayPoint()内でManagerへの登録まで完了している
			EditorManager::Instance().SetSelectedObject(aabbBox);

			EditorManager::Instance().MarkDirty();
		}
	}


	ImGui::SameLine();

	bool isDebug = AABBCollisionManager::Instance().IsDebug();

	if (ImGui::Checkbox("Debug", &isDebug))
	{
		isDebug? AABBCollisionManager::Instance().SetDebugFlg(true):
			     AABBCollisionManager::Instance().SetDebugFlg(false);
	}

}

void Hierarchy::AddOBB()
{
	if (ImGui::Button("Add OBB"))
	{
		auto obb = OBBCollisionManager::Instance().CreateOBBCollision();

		if (obb)
		{
			// CreateWayPoint()内でManagerへの登録まで完了している
			EditorManager::Instance().SetSelectedObject(obb);

			EditorManager::Instance().MarkDirty();
		}
	}

	ImGui::SameLine();

	bool isDebug = OBBCollisionManager::Instance().IsDebug();

	if (ImGui::Checkbox("Debug", &isDebug))
	{
		isDebug ? OBBCollisionManager::Instance().SetDebugFlg(true) :
			OBBCollisionManager::Instance().SetDebugFlg(false);
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

	case HierarchyCategory::CollisionBox:
		DrawCollisionBox();
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
	for (const auto& wayPoint : WayPointManager::Instance().GetWayPoints())
	{
		if (!wayPoint)
		{
			continue;
		}

		SelectHierarchyObject(wayPoint);
	}
}

void Hierarchy::DrawStage()
{
	DrawObjectList(KdGameObject::ObjectCategory::Stage);

	ImGui::Separator();

	DrawObjectList(KdGameObject::ObjectCategory::Gimmick);
}

void Hierarchy::DrawCollisionBox()
{

	for (const auto& aabbBox : AABBCollisionManager::Instance().GetAABBCollisionList())
	{
		if (!aabbBox)
		{
			continue;
		}

		SelectHierarchyObject(aabbBox);
	}
}

void Hierarchy::DrawOBB()
{
	for (const auto& obb : OBBCollisionManager::Instance().GetOBBCollisionList())
	{
		if (!obb)
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
