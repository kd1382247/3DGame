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

#include"../../GameObject/PointLight/PointLight.h"
#include"../../GameObject/PointLight/PointLightManager.h"

#include"../../GameObject/Stage/StageObject.h"
#include"../../GameObject/Gimmick/StageGimmick.h"
#include"../../System/StageLoder/StageLoder.h"

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


	// ステージモデル単位で保存するデータ(同じモデルは最初のステージでだけ編集できる)
	const bool isStageDataCategory =
		m_category == HierarchyCategory::AABB ||
		m_category == HierarchyCategory::OBB ||
		m_category == HierarchyCategory::WayPoint ||
		m_category == HierarchyCategory::PointLight;

	// Active Stageで一覧を絞り込むカテゴリ(ギミック・AreaVolumeはステージごとに保存するので、編集の制限はない)
	const bool isOwnerCategory =
		isStageDataCategory ||
		m_category == HierarchyCategory::Gimmick ||
		m_category == HierarchyCategory::AreaVolume;

	if (isOwnerCategory)
	{
		ImGui::Separator();
		DrawActiveStageSelector();
	}

	if (isStageDataCategory)
	{
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
		CategorySelectItem("AreaVolume", HierarchyCategory::AreaVolume);
		CategorySelectItem("WayPoint",   HierarchyCategory::WayPoint);
		CategorySelectItem("AABB",       HierarchyCategory::AABB);
		CategorySelectItem("OBB",        HierarchyCategory::OBB);
		CategorySelectItem("PointLight", HierarchyCategory::PointLight);

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
	case HierarchyCategory::AreaVolume:   return "AreaVolume";
	case HierarchyCategory::WayPoint:     return "WayPoint";
	case HierarchyCategory::AABB:         return "AABB";
	case HierarchyCategory::OBB:          return "OBB";
	case HierarchyCategory::PointLight: return "PointLight";
	}

	return "";
}

void Hierarchy::DrawActiveStageSelector()
{
	// ギミック・AreaVolumeは通路には置かないので、通路を候補に入れない
	const bool excludeCorridor =
		(m_category == HierarchyCategory::Gimmick) ||
		(m_category == HierarchyCategory::AreaVolume);

	auto isCandidate = [excludeCorridor](const std::shared_ptr<StageObject>& stage)
	{
		return !(excludeCorridor && stage->IsCorridor());
	};

	auto activeStage = EditorManager::Instance().GetActiveStage();

	// 他のカテゴリで通路が選ばれたままなら、いったん外して選び直す
	if (activeStage && !isCandidate(activeStage))
	{
		activeStage = nullptr;
		EditorManager::Instance().SetActiveStage(nullptr);
		EditorManager::Instance().SetSelectedObject(nullptr);
	}

	if (!activeStage)
	{
		for (const auto& obj : SceneManager::Instance().GetObjList())
		{
			auto stage= std::dynamic_pointer_cast<StageObject>(obj);
			if (stage && isCandidate(stage))
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
			if (stage && isCandidate(stage))
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
					PointLightObjectManager::Instance().SetIsDebug(false);
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
	case HierarchyCategory::AreaVolume:
		AddAreaVolume();
		break;
	case HierarchyCategory::AABB:
		AddAABB();
		break;
	case HierarchyCategory::OBB:
		AddOBB();
		break;
	case HierarchyCategory::PointLight:
		AddPointLight();
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
		// 追加できるステージモデルの一覧(Inspectorのモデル変更と同じ一覧)
		const auto modelNames = StageLoder::Instance().GetAvailableModelNames();

		if (modelNames.empty())
		{
			ImGui::TextDisabled(U8("ステージモデルがありません"));
		}

		for (const auto& modelName : modelNames)
		{
			if (ImGui::Selectable(
				modelName.c_str(),
				false,
				ImGuiSelectableFlags_DontClosePopups))
			{
				// 選んだモデルのステージを作る
				EditorManager::Instance().CreateGameObject("StageObject", modelName);

				EditorManager::Instance().MarkDirty();
			}
		}

		ImGui::EndPopup();
	}
}

void Hierarchy::AddGimmick()
{
	// ギミックを新規作成
	// ギミックはActive Stageの持ち物になるので、ステージが無いときは作れない
	if (EditorManager::Instance().GetActiveStage())
	{
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
	else
	{
		ImGui::TextDisabled(U8("先に部屋のステージを追加してください"));
	}

	ImGui::SameLine();

	// ギミックのデバッグ表示のON/OFF
	bool isDebug = EditorManager::Instance().IsGimmickDebug();

	if (ImGui::Checkbox("Debug", &isDebug))
	{
		EditorManager::Instance().SetGimmickDebug(isDebug);
	}
}

void Hierarchy::AddAreaVolume()
{
	// AreaVolumeを新規作成(種類は1つだけなので、ボタンを押すとそのまま作る)
	// Active Stageの持ち物になるので、ステージが無いときは作れない
	if (EditorManager::Instance().GetActiveStage())
	{
		if (ImGui::Button("Add AreaVolume"))
		{
			EditorManager::Instance().CreateGameObject("AreaVolume");

			EditorManager::Instance().MarkDirty();
		}
	}
	else
	{
		ImGui::TextDisabled(U8("先に部屋のステージを追加してください"));
	}

	ImGui::SameLine();

	// AreaVolumeのデバッグ表示のON/OFF(Gimmickとは別)
	bool isDebug = EditorManager::Instance().IsAreaVolumeDebug();

	if (ImGui::Checkbox("Debug", &isDebug))
	{
		EditorManager::Instance().SetAreaVolumeDebug(isDebug);
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

void Hierarchy::AddPointLight()
{
	// PointLightはActive Stageの持ち物になるので、ステージが無いときは作れない
	if (EditorManager::Instance().GetActiveStage())
	{
		if (ImGui::Button("Add PointLight"))
		{
			auto light = PointLightObjectManager::Instance().CreatePointLight(
				EditorManager::Instance().GetActiveStage());

			if (light)
			{
				// CreatePointLight()内でシーンへの追加まで完了している
				EditorManager::Instance().SetSelectedObject(light);

				EditorManager::Instance().MarkDirty();
			}
		}
	}
	else
	{
		ImGui::TextDisabled(U8("先に部屋のステージを追加してください"));
	}

	ImGui::SameLine();

	bool isDebug = PointLightObjectManager::Instance().IsDebug();

	if (ImGui::Checkbox("Debug", &isDebug))
	{
		PointLightObjectManager::Instance().SetIsDebug(isDebug);
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

	case HierarchyCategory::AreaVolume:
		DrawAreaVolume();
		break;

	case HierarchyCategory::AABB:
		DrawAABB();
		break;

	case HierarchyCategory::OBB:
		DrawOBB();
		break;

	case HierarchyCategory::PointLight:
		DrawPointLight();
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
	auto activeStage = EditorManager::Instance().GetActiveStage();

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		if (!obj ||
			obj->GetObjectCategory() != KdGameObject::ObjectCategory::Gimmick)
		{
			continue;
		}

		// 持ち主がActive Stageではないギミックは出さない
		// (持ち主の仕組みを持たないギミックは、常に出す)
		auto gimmick = std::dynamic_pointer_cast<StageGimmick>(obj);

		if (gimmick &&
			gimmick->GetOwnerStage() != activeStage)
		{
			continue;
		}

		SelectHierarchyObject(obj);
	}
}

void Hierarchy::DrawAreaVolume()
{
	auto activeStage = EditorManager::Instance().GetActiveStage();

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		if (!obj ||
			obj->GetObjectCategory() != KdGameObject::ObjectCategory::AreaVolume)
		{
			continue;
		}

		// 持ち主がActive Stageではないものは出さない
		auto areaVolume = std::dynamic_pointer_cast<StageGimmick>(obj);

		if (areaVolume &&
			areaVolume->GetOwnerStage() != activeStage)
		{
			continue;
		}

		SelectHierarchyObject(obj);
	}
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

void Hierarchy::DrawPointLight()
{
	auto activeStage = EditorManager::Instance().GetActiveStage();

	for (const auto& light : PointLightObjectManager::Instance().GetPointLights())
	{
		if (!light ||
			light->GetOwner() != activeStage)
		{
			continue;
		}

		SelectHierarchyObject(light);
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
