#include "Hierarchy.h"

#include "../EditorManager.h"
#include "../../Scene/SceneManager.h"
#include "../../../Framework/GameObject/KdGameObjectFactory.h"

#include"../../GameObject/Stage/WayPoint/WayPoint.h"
#include"../../GameObject/Stage/Collision/AABBCollision/AABBCollision.h"
#include"../../GameObject/Stage/Collision/OBBCollision/OBBCollision.h"
#include"../../GameObject/Stage/Collision/AIBlockCollision/AIBlock.h"
#include"../../GameObject/Stage/PointLight/PointLight.h"

#include"../../GameObject/Stage/StageObject.h"
#include"../../GameObject/Gimmick/StageGimmick.h"
#include"../../System/StageLoder/StageLoder.h"

#include"../StageOwnedTypeInfo/StageOwnedTypeInfo.h"

#include<algorithm>
#include<cctype>

namespace
{

	// クラスと番号に分ける
	std::pair<std::string, int> SplitObjectName(const std::string& name)
	{
		size_t stPos = name.rfind('_');

		// 文字無かったら
		if (stPos == std::string::npos)
		{
			return { name,-1 };
		}

		// 見つかったら文字を分ける

		std::string className = name.substr(0,stPos);
		std::string id = name.substr(stPos+1);

		// 文字を数字に変換する
		int number = -1;

		// 変換の結果を入れる
		auto result = std::from_chars(id.data(), id.data() + id.size(), number);

		// 成功したかつ、ptrが末尾なら
		if (result.ec == std::errc()&&
			result.ptr==id.data()+id.size())
		{
			return { className,number };
		}

		// 変換に失敗したら
		return { name,-1 };

	}

	// 2つの名前を比べる
	bool IsObjectNameLess(const std::string& nameA, const std::string& nameB)
	{
		return SplitObjectName(nameA) < SplitObjectName(nameB);
	}


}


void Hierarchy::OnDraw()
{

	// カテゴリ選択(ボタンを押すと一覧が出て選択でき、選んだカテゴリ名をボタンの隣に表示する)
	DrawCategorySelector();

	// ステージモデル単位で保存するデータ(同じモデルは最初のステージでだけ編集できる)
	const bool isStageDataCategory = FindStageOwnedTypeInfo(m_category) != nullptr;

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
}

ImGuiWindowFlags Hierarchy::GetImGuiWindowFlags() const
{

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse;

	return flags;
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
		CategorySelectItem("CheckPoint", HierarchyCategory::CheckPoint);
		CategorySelectItem("WayPoint",   HierarchyCategory::WayPoint);
		CategorySelectItem("AABB",       HierarchyCategory::AABB);
		CategorySelectItem("OBB",        HierarchyCategory::OBB);
		CategorySelectItem("AIBlock",    HierarchyCategory::AIBlock);
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
	case HierarchyCategory::CheckPoint:   return "CheckPoint";
	case HierarchyCategory::WayPoint:     return "WayPoint";
	case HierarchyCategory::AABB:         return "AABB";
	case HierarchyCategory::OBB:          return "OBB";
	case HierarchyCategory::AIBlock:      return "AIBlock";
	case HierarchyCategory::PointLight:   return "PointLight";
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

					// 表を参照で取得
					for (const auto& table: GetStageOwnedTypeInfoTable())
					{
						// デバッグ表示を消す
						table.second.setDebug(false);
					}
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
	case HierarchyCategory::Stage:
		AddStage();
		break;
	case HierarchyCategory::Gimmick:
		AddGimmick();
		break;
	case HierarchyCategory::AreaVolume:
		AddAreaVolume();
		break;
	case HierarchyCategory::CheckPoint:
		AddCheckPoint();
		break;
	case HierarchyCategory::WayPoint:
	case HierarchyCategory::AABB:
	case HierarchyCategory::OBB:
	case HierarchyCategory::AIBlock:
	case HierarchyCategory::PointLight:
		AddStageOwned();
		break;
	}
}

void Hierarchy::AddStageOwned()
{

	// カテゴリごとの構造体を取得
	auto info = FindStageOwnedTypeInfo(m_category);

	if (info == nullptr)
	{
		return;
	}

	auto activeStage = EditorManager::Instance().GetActiveStage();

	if (activeStage)
	{
		// カテゴリでラベル名をセット
		if (ImGui::Button(GetCategoryLabel(m_category)))
		{
			auto stageOwnedObj = info->createObject(activeStage);

			if (stageOwnedObj)
			{
				EditorManager::Instance().SetSelectedObject(stageOwnedObj);

				EditorManager::Instance().MarkDirty();
			}
		}
	}
	else
	{
		ImGui::TextDisabled(U8("先に部屋のステージを追加してください"));
	}


	ImGui::SameLine();

	bool isDebug = info->isDebug();

	if (ImGui::Checkbox("Debug", &isDebug))
	{
		info->setDebug(isDebug);
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

void Hierarchy::AddCheckPoint()
{
	// CheckPointを新規作成(種類は1つだけなので、ボタンを押すとそのまま作る)
	// ステージには属さないので、Active Stageが無くても作れる
	if (ImGui::Button("Add CheckPoint"))
	{
		EditorManager::Instance().CreateGameObject("CheckPoint");

		EditorManager::Instance().MarkDirty();
	}

	ImGui::SameLine();

	// CheckPointのデバッグ表示のON/OFF(Gimmickとは別)
	bool isDebug = EditorManager::Instance().IsCheckPointDebug();

	if (ImGui::Checkbox("Debug", &isDebug))
	{
		EditorManager::Instance().SetCheckPointDebug(isDebug);
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
	case HierarchyCategory::Stage:
		DrawStage();
		break;

	case HierarchyCategory::Gimmick:
		DrawGimmick();
		break;

	case HierarchyCategory::AreaVolume:
		DrawAreaVolume();
		break;

	case HierarchyCategory::CheckPoint:
		DrawCheckPoint();
		break;

	case HierarchyCategory::WayPoint:
	case HierarchyCategory::AABB:
	case HierarchyCategory::OBB:
	case HierarchyCategory::AIBlock:
	case HierarchyCategory::PointLight:
		DrawStageOwnedObjects();
		break;
	}
}

void Hierarchy::DrawStageOwnedObjects()
{
	auto activeStage = EditorManager::Instance().GetActiveStage();

	auto info = FindStageOwnedTypeInfo(m_category);

	if (info == nullptr)
	{
		return;
	}
	// オブジェクトリストを移す
	auto objects = info->getObjects();

	// 配列の並びをIDでソートする
	std::sort(objects.begin(), objects.end(),
		[](const std::shared_ptr<StageOwnedObject>& a, const std::shared_ptr<StageOwnedObject>& b)
		{
			return a->GetID() < b->GetID();
		});


	for (const auto& ownedObj : objects)
	{
		if (!ownedObj)
		{
			continue;
		}

		// ownerが一致しない場合は飛ばす
		if (ownedObj->GetParent() != activeStage)
		{
			continue;
		}

		SelectHierarchyObject(ownedObj);
	}

}

void Hierarchy::DrawStageGimmickList(KdGameObject::ObjectCategory objCategory)
{
	std::vector<std::shared_ptr<StageGimmick>> gimmickList;

	// ギミックオブジェクトを集める
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto gimmick = std::dynamic_pointer_cast<StageGimmick>(obj);

		if (!gimmick) { continue; }

		// カテゴリが違ったり、現在編集中の親じゃない場合飛ばす
		if (gimmick->GetObjectCategory() != objCategory||
			gimmick->GetParent()!=EditorManager::Instance().GetActiveStage())
		{
			continue;
		}

		gimmickList.push_back(gimmick);
	}

	// 名前で比べてソートする
	std::sort(gimmickList.begin(), gimmickList.end(),
		[](const std::shared_ptr<StageGimmick>& a, const std::shared_ptr<StageGimmick>& b)
		{
			return IsObjectNameLess(a->GetObjectName(),b->GetObjectName());
		});
	
	// 一覧表示
	for (const auto& gimmick : gimmickList)
	{
		SelectHierarchyObject(gimmick);
	}
}

void Hierarchy::DrawGameObjects()
{
	DrawObjectList(KdGameObject::ObjectCategory::Character);
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
	DrawStageGimmickList(KdGameObject::ObjectCategory::Gimmick);
}

void Hierarchy::DrawAreaVolume()
{
	DrawStageGimmickList(KdGameObject::ObjectCategory::AreaVolume);
}

void Hierarchy::DrawCheckPoint()
{
	// ステージには属さないので、Active Stageに関係なく全部出す
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		if (!obj ||
			obj->GetObjectCategory() != KdGameObject::ObjectCategory::CheckPoint)
		{
			continue;
		}

		SelectHierarchyObject(obj);
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
