#include "EditorManager.h"

#include"../../Framework/Effekseer/KdEffekseerManager.h"
#include"../GameObject/Camera/CameraBase.h"
#include "../../Framework/GameObject/KdGameObjectFactory.h"

#include"EditorPanel/EditorPanel.h"

#include"../GameObject/Stage/WayPoint/WayPoint.h"

#include"../GameObject/Stage/Collision/AABBCollision/AABBCollision.h"

#include"../GameObject/Stage/Collision/OBBCollision/OBBCollision.h"

#include"../GameObject/Stage/Collision/AIBlockCollision/AIBlock.h"

#include"../GameObject/Stage/PointLight/PointLight.h"

#include"../Scene/EditorScene/EditorScene.h"

#include"../GameObject/Stage/StageObject.h"
#include"../GameObject/Gimmick/StageGimmick.h"
#include"../GameObject/Gimmick/Checkpoint/CheckPointManager.h"

#include"../System/ReferenceManager/ReferenceManager.h"
#include "../Scene/SceneManager.h"
#include"../System/StageDataManager/StageDataManager.h"

#include"../BGMManager/BGMManager.h"

#include "Hierarchy/Hierarchy.h"
#include "Inspector/Inspector.h"
#include "StageEditor/StageEditor.h"
#include"SceneView/SceneView.h"

#include"StageOwnedTypeInfo/StageOwnedTypeInfo.h"

void EditorManager::Init()
{
	if(!m_spHierarchy)
	{
		m_spHierarchy = std::make_shared<Hierarchy>();
		m_panelList.push_back(m_spHierarchy);
	}

	if (!m_spInspector)
	{
		m_spInspector = std::make_shared<Inspector>();
		m_panelList.push_back(m_spInspector);
	}

	if (!m_spSceneView)
	{
		m_spSceneView = std::make_shared<SceneView>();
		m_spSceneView->Init();

		m_panelList.push_back(m_spSceneView);
	}

	if (!m_spStageEditor)
	{
		m_spStageEditor = std::make_shared<StageEditor>();
	}

	if (!m_spMessageWindow)
	{
		m_spMessageWindow = std::make_shared<MessageWindow>();
	}


	auto spCamera = m_wpEditorCamera.lock();
	if (!spCamera)
	{
		return;
	}
	float aspect = m_spSceneView->GetAspectRatio();
	spCamera->GetCamera()->SetProjectionMatrix(60.0f, 2000.0f, 0.01, aspect);

}

void EditorManager::Draw()
{
	if(m_editorMode==EditorMode::Edit)
	{
		DrawToolBar();

		for (const auto& panel : m_panelList)
		{
			if (!panel)
			{
				continue;
			}

			panel->Draw();
		}

		if (m_spSceneView->SizeChanged())
		{
			UpdateSceneViewProjection();
		}

		UpdateMouseSelection();


	}

	if (m_editorMode == EditorMode::Play)
	{
		if (GetAsyncKeyState(VK_TAB) & 0x8000)
		{
			StopPlayMode();
		}
	}


	m_spMessageWindow->Draw();
}

void EditorManager::StartPlayMode()
{

	// Play開始/終了でステージが作り直されるため、編集中のステージを名前で覚えておく
	// (覚えておかないとActiveStageが消え、Hierarchyが先頭のステージを選び直してしまう)
	const auto currentActiveStage = GetActiveStage();
	const std::string activeStageName = currentActiveStage ? currentActiveStage->GetObjectName() : std::string();

	if (!StageDataManager::Instance().SaveTemporary())
	{
		return;
	}

	auto editorScene = SceneManager::Instance().GetCurrentScene<EditorScene>();
	if (!editorScene)
	{
		return;
	}

	// Play中の状態を削除
	editorScene->Backup();


	// Edit開始前の状態を復元
	if (!StageDataManager::Instance().LoadTemporary())
	{
		// ロードに失敗したため復元する
		editorScene->Restore();

		return;
	}

	// バックアップリストをクリア
	editorScene->Discard();

	// 作り直されたステージの中から、編集中だったステージを選び直す
	RestoreActiveStageByName(activeStageName);

	// 前のPlayで通ったチェックポイントの記録を消す (Playは毎回、最初から)
	CheckPointManager::Instance().Reset();

	// モードを切り替える
	SetEditorMode(EditorMode::Play);

	// 選択中のオブジェクトを解除
	SetSelectedObject(nullptr);

	// 全てのオブジェクトを生成後に関連付け
	ReferenceManager::Instance().AssociateObjects(SceneManager::Instance().GetObjList());

	// マウスカーソル
	ShowCursor(false);
}

void EditorManager::StopPlayMode()
{

	// Play開始/終了でステージが作り直されるため、編集中のステージを名前で覚えておく
	// (覚えておかないとActiveStageが消え、Hierarchyが先頭のステージを選び直してしまう)
	const auto currentActiveStage = GetActiveStage();
	const std::string activeStageName = currentActiveStage ? currentActiveStage->GetObjectName() : std::string();

	auto editorScene = SceneManager::Instance().GetCurrentScene<EditorScene>();
	if (!editorScene)
	{
		return;
	}

	// 音を全て止める
	KdAudioManager::Instance().StopAllSound();

	// BGMの状態(今のBGM・上書き)もリセット
	BGMManager::Instance().Reset();

	// Play中の状態を削除
	editorScene->Backup();

	// Edit開始前の状態を復元
	if (!StageDataManager::Instance().LoadTemporary())
	{
		// ロードに失敗したため復元する
		editorScene->Restore();

		return;
	}

	// バックアップリストをクリア
	editorScene->Discard();

	// 作り直されたステージの中から、編集中だったステージを選び直す
	RestoreActiveStageByName(activeStageName);

	// モードを切り替える
	SetEditorMode(EditorMode::Edit);

	// エフェクトをストップ
	KdEffekseerManager::GetInstance().StopAllEffect();

	// カラースフィアを消す
	KdShaderManager::Instance().ClearColorSphere();

	// マウスカーソル
	ShowCursor(true);
}

void EditorManager::RestoreActiveStageByName(const std::string& stageName)
{
	if (stageName.empty())
	{
		return;
	}

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto stage = std::dynamic_pointer_cast<StageObject>(obj);

		if (stage && stage->GetObjectName() == stageName)
		{
			SetActiveStage(stage);
			return;
		}
	}
}

void EditorManager::BeginSceneViewRender()
{
	m_spSceneView->BeginRender();
}

void EditorManager::EndSceneViewRender()
{
	m_spSceneView->EndRender();
}



void EditorManager::DrawToolBar()
{

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse|
		ImGuiWindowFlags_MenuBar;

	ImGui::Begin("Menu",nullptr,flags);

	// メニューバー表示
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Window"))
		{
			for (const auto& panel : m_panelList)
			{
				if (!panel)
				{
					continue;
				}

				if (ImGui::MenuItem(panel->GetWindowName().c_str(),nullptr,panel->IsVisible()))
				{

					panel->IsVisible() ?
						panel->SetVisible(false) : panel->SetVisible(true);

				}

			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	//-----------------------------
	//  左:ファイル操作
	//-----------------------------
	m_spStageEditor->Draw();


	ImGui::SameLine();

	ImGui::TextDisabled("|");

	ImGui::SameLine();

	//-----------------------------
	//  中央:ステージ名
	//-----------------------------

	// Saveボタンの右端から少し空ける
	const float stageStartX =ImGui::GetCursorPosX() + 10.0f;

	// Playボタン用の幅を右側に確保
	const float playButtonWidth = 70.0f;
	const float rightMargin = 10.0f;

	const float windowWidth =ImGui::GetWindowWidth();

	const float stageEndX =windowWidth/2 -playButtonWidth -rightMargin -20.0f;

	// Stage名を描画できる幅
	const float stageWidth =stageEndX - stageStartX;


	ImGui::SetCursorPosX(stageStartX);
	
	std::string stageName = m_spStageEditor->GetCurrentStageName();

	// ステージ名表示
	ImGui::Text("Stage : %s",stageName.empty()? "None": stageName.c_str());

	ImGui::SameLine();
	//-----------------------------
	//  右:プレイ
	//-----------------------------

	const float playStartX = ImGui::GetCursorPosX()+10.0f;

	ImGui::SetCursorPosX(playStartX);

	if (ImGui::Button("Play", ImVec2(playButtonWidth, 0.0f)))
	{
		if(m_spStageEditor->IsEditStage())
		{
			StartPlayMode();
		}
		else
		{
			m_spMessageWindow->Open(U8("エラー"), U8("プレイするステージがありません"));
		}
	}

	ImGui::End();
}

std::string EditorManager::MakeEllipsisText(const std::string& text, float maxWidth)
{
	if (text.empty())
	{
		return text;
	}

	// そのまま収まるなら変更しない
	if (ImGui::CalcTextSize(text.c_str()).x <= maxWidth)
	{
		return text;
	}

	const std::string ellipsis = "...";

	std::string result = text;

	// ...を付けても maxWidth に収まるまで後ろから削る

	while (!result.empty())
	{
		result.pop_back();

		const std::string displayText = result + ellipsis;

		if (ImGui::CalcTextSize(displayText.c_str()).x <= maxWidth)
		{
			return displayText;
		}
	}

	return ellipsis;
}

KdCollider::RayInfo EditorManager::CreateRayInfo(UINT type)
{

	// SceneView内のマウス座標
	ImVec2 sceneMousePos =
		m_spSceneView->GetLocalMousePos();

	// SceneViewのウィンドウ幅
	float sceneWidth = static_cast<float>( m_spSceneView->GetWidth());
	float sceneHeight = static_cast<float>(m_spSceneView->GetHeight());

	// マウス座標を取得
	POINT mousePos;
	mousePos.x =static_cast<LONG>(sceneMousePos.x);
	mousePos.y =static_cast<LONG>(sceneMousePos.y);


	// マウスを3D座標へ変換する
	auto spCamera = m_wpEditorCamera.lock()->GetCamera();

	if (!spCamera)
	{
		return{};
	}

	Math::Vector3 rayPos = spCamera->GetCameraMatrix().Translation();
	Math::Vector3 rayDir = Math::Vector3::Zero;
	float         range = 2000.f;

	spCamera->GenerateRayInfoFromClientPos(
		mousePos,
		rayPos,
		rayDir,
		range,
		sceneWidth,
		sceneHeight
	);

	// 生成したレイ情報でオブジェクトの当たり判定を行う
	KdCollider::RayInfo rayInfo;

	rayInfo.m_dir = rayDir;
	rayInfo.m_pos = rayPos;
	rayInfo.m_range = range;
	rayInfo.m_type = type;

	return rayInfo;
}

void EditorManager::UpdateSceneViewProjection()
{
	auto spCamera = m_wpEditorCamera.lock();

	if (!spCamera)
	{
		return;
	}

	float aspect = m_spSceneView->GetAspectRatio();

	spCamera->GetCamera()->SetProjectionMatrix(60.0f, 2000.0f, 0.01, aspect);
}

void EditorManager::UpdateMouseSelection()
{
	if (!IsEditMode()){return;}
	if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) { return; }
	if (!m_spSceneView->IsHovered()) { return; }


	switch (m_spHierarchy->GetHierarchyCategory())
	{
	case HierarchyCategory::GameObject :

		SelectGameObjectByMouse();
		break;

	case HierarchyCategory::Stage:
		SelectStageObjectByMouse();
		break;

	case HierarchyCategory::Gimmick:
		SelectGimmickByMouse();
		break;

	case HierarchyCategory::AreaVolume:
		SelectAreaVolumeByMouse();
		break;

	case HierarchyCategory::CheckPoint:
		SelectCheckPointByMouse();
		break;

	case HierarchyCategory::WayPoint:
	case HierarchyCategory::AABB:
	case HierarchyCategory::OBB:
	case HierarchyCategory::AIBlock:
	case HierarchyCategory::PointLight:

		SelectStageOwnedByMouse();

		break;

	}
}

template<typename Container, typename Predicate>
std::shared_ptr<KdGameObject> EditorManager::SelectClosestByMouse(const Container& objects, UINT rayType, Predicate predicate)
{

	KdCollider::RayInfo rayInfo = CreateRayInfo(rayType);

	float maxOverlap = 0.0f;

	std::shared_ptr<KdGameObject> selectedObj = nullptr;

	for (const auto& obj : objects)
	{
		if (!obj || !predicate(obj))
		{
			continue;
		}

		std::list<KdCollider::CollisionResult> retRayList;

		if (!obj->Intersects(rayInfo, &retRayList))
		{
			continue;
		}

		for (const auto& ret : retRayList)
		{
			// レイを遮断しオーバーした長さが
			// 一番長いものを探す
			if (maxOverlap < ret.m_overlapDistance)
			{
				maxOverlap = ret.m_overlapDistance;

				selectedObj = obj;
			}
		}
	}

	return selectedObj;
}


void EditorManager::SelectStageOwnedByMouse()
{

	if (!CanEditStageData())
	{
		return;
	}

	// 現在のカテゴリの構造体を受け取る
	auto info = FindStageOwnedTypeInfo(m_spHierarchy->GetHierarchyCategory());

	if (info == nullptr)
	{
		return;
	}

	// 現在編集中のステージをセット
	auto activeStage = GetActiveStage();

	auto stageOwnedObjList = info->getObjects();

	// マウスとオブジェクトが一致したら、そのオブジェクトを返す
	auto selectedObj = SelectClosestByMouse(
		stageOwnedObjList,
		KdCollider::TypeBump,
		// リストの中で現在の編集中のステージが親のオブジェクトをチェックする
		[&activeStage](const std::shared_ptr<StageOwnedObject>& obj)
		{
			return obj->GetOwner() == activeStage;
		});

	// 選択したオブジェクトをセット
	SetSelectedObject(selectedObj);
}

void EditorManager::SelectGameObjectByMouse()
{

	auto selectedObj = SelectClosestByMouse(
		SceneManager::Instance().GetObjList(),
		KdCollider::TypeBump,
		[](const std::shared_ptr<KdGameObject>& obj)
		{
			return obj->GetObjectCategory() == KdGameObject::ObjectCategory::Character;
		});

	SetSelectedObject(selectedObj);
}

void EditorManager::SelectStageObjectByMouse()
{
	auto selectedObj = SelectClosestByMouse(
		SceneManager::Instance().GetObjList(),
		KdCollider::TypeEvent,
		[](const std::shared_ptr<KdGameObject>& obj)
		{
			return obj->GetObjectCategory() == KdGameObject::ObjectCategory::Stage;
		});

	SetSelectedObject(selectedObj);
}

void EditorManager::SelectGimmickByMouse()
{
	auto activeStage = GetActiveStage();

	// 優先度の高いギミックから順に調べる
	// (Roomのように大きな箱は優先度を低くしてあり、他のギミックに当たっていないときだけ選ばれる)
	const int priorities[] = { 0, -1 };

	std::shared_ptr<KdGameObject> selectedObj = nullptr;

	for (const int priority : priorities)
	{
		selectedObj = SelectClosestByMouse(
			SceneManager::Instance().GetObjList(),
			KdCollider::TypeEvent | KdCollider::TypeBump,
			[activeStage, priority](const std::shared_ptr<KdGameObject>& obj)
			{
				if (obj->GetObjectCategory() != KdGameObject::ObjectCategory::Gimmick)
				{
					return false;
				}

				// 持ち主がいるギミックは、Active Stageのものだけ選べる(Hierarchyの一覧と同じ)
				auto gimmick = std::dynamic_pointer_cast<StageGimmick>(obj);

				if (!gimmick)
				{
					return priority == 0;
				}

				return gimmick->GetOwnerStage() == activeStage
					&& gimmick->GetSelectPriority() == priority;
			});

		if (selectedObj)
		{
			break;
		}
	}

	SetSelectedObject(selectedObj);
}

void EditorManager::SelectAreaVolumeByMouse()
{
	auto activeStage = GetActiveStage();

	// AreaVolumeだけが対象なので、Roomなど他のギミックに遮られない
	auto selectedObj = SelectClosestByMouse(
		SceneManager::Instance().GetObjList(),
		KdCollider::TypeBump,
		[activeStage](const std::shared_ptr<KdGameObject>& obj)
		{
			if (obj->GetObjectCategory() != KdGameObject::ObjectCategory::AreaVolume)
			{
				return false;
			}

			// Active Stageのものだけ選べる(Hierarchyの一覧と同じ)
			auto areaVolume = std::dynamic_pointer_cast<StageGimmick>(obj);

			return areaVolume && areaVolume->GetOwnerStage() == activeStage;
		});

	SetSelectedObject(selectedObj);
}

void EditorManager::SelectCheckPointByMouse()
{
	// CheckPointだけが対象なので、Roomなど他のギミックに遮られない
	// (ステージには属さないので、Active Stageに関係なく選べる。Hierarchyの一覧と同じ)
	auto selectedObj = SelectClosestByMouse(
		SceneManager::Instance().GetObjList(),
		KdCollider::TypeBump,
		[](const std::shared_ptr<KdGameObject>& obj)
		{
			return obj->GetObjectCategory() == KdGameObject::ObjectCategory::CheckPoint;
		});

	SetSelectedObject(selectedObj);
}

void EditorManager::CreateGameObject(const std::string& className, const std::string& stageModelName)
{
	auto newObject =
		KdGameObjectFactory::Instance().CreateGameObject(className);

	if (!newObject)
	{
		OutputDebugStringA("オブジェクトの生成に失敗しました\n");
		KdDebugGUI::Instance().AddErrorLog("オブジェクトの生成に失敗しました\n");
		return;
	}

	// ステージオブジェクトに変換
	auto stage = std::dynamic_pointer_cast<StageObject>(newObject);

	// 選んだステージモデルを、Init()で読み込まれるようにInit()より先にセットする
	if (stage && !stageModelName.empty())
	{
		stage->SetStageModelName(stageModelName);
	}

	newObject->Init();

	// ファクトリー登録名を保持しておく(SaveDataの"Class"に使う。表示名とは別)
	newObject->SetFactoryClassName(className);


	std::string baseName = className;

	if (stage)
	{
		baseName = stage->GetStageModelName();
		
	}

	// 表示名は"Slime_0"のように自動採番する
	newObject->SetObjectName(MakeUniqueObjectName(baseName));
	

	// ギミックは、今のActive Stageを持ち主にする
	if (auto gimmick = std::dynamic_pointer_cast<StageGimmick>(newObject))
	{
		gimmick->SetOwnerStage(GetActiveStage());
	}

	SceneManager::Instance().AddObject(newObject);

	if (stage)
	{
		StageDataManager::Instance().LoadStageModelData(stage);
		SetActiveStage(stage);
	}

	// 現在選択中のオブジェクト
	SetSelectedObject(newObject);
}

bool EditorManager::CanCopyObject(const std::shared_ptr<KdGameObject>& obj) const
{
	if (!obj)
	{
		return false;
	}

	return std::dynamic_pointer_cast<StageGimmick>(obj)
		|| std::dynamic_pointer_cast<AABBCollision>(obj)
		|| std::dynamic_pointer_cast<OBBCollision>(obj)
		|| std::dynamic_pointer_cast<AIBlock>(obj)
		|| std::dynamic_pointer_cast<PointLightObject>(obj)
		|| std::dynamic_pointer_cast<WayPoint>(obj)
		|| obj->GetObjectCategory() == KdGameObject::ObjectCategory::CheckPoint;
}

std::shared_ptr<KdGameObject> EditorManager::CopyObject(const std::shared_ptr<KdGameObject>& src)
{
	if (!src)
	{
		return nullptr;
	}

	auto ownedObject = std::dynamic_pointer_cast<StageOwnedObject>(src);

	if (ownedObject)
	{
		for (const auto &table:GetStageOwnedTypeInfoTable())
		{
			if (table.second.isType(ownedObject))
			{
				auto copy = table.second.createObject(ownedObject->GetOwner());

				if (!copy)
				{
					return nullptr;
				}

				copy->CopyFrom(ownedObject);
				return copy;
			}
		}
	}


	//------------------------------------------------------------
	// CheckPoint: 同じクラスを作り、保存データ(SaveData)の内容を読み込ませて写す
	// (ステージには属さないので、持ち主は写さない)
	//------------------------------------------------------------
	if (src->GetObjectCategory() == KdGameObject::ObjectCategory::CheckPoint)
	{
		const std::string className = src->GetFactoryClassName();

		// 未登録のクラス名はファクトリーがassertで止まってしまうので、先に調べる
		if (!KdGameObjectFactory::Instance().GetCreateFunctions().contains(className))
		{
			KdDebugGUI::Instance().AddErrorLog("Copy failed : unknown class '%s'\n", className.c_str());
			return nullptr;
		}

		auto copy = KdGameObjectFactory::Instance().CreateGameObject(className);

		if (!copy)
		{
			KdDebugGUI::Instance().AddErrorLog("Copy failed : create object\n");
			return nullptr;
		}

		copy->Init();

		copy->SetFactoryClassName(className);

		// 表示名は、空いている最小の番号で付ける
		copy->SetObjectName(MakeUniqueObjectName(className));

		// 位置・大きさ・回転を元と同じにする
		copy->SetScale(src->GetScale());
		copy->SetRotation(src->GetRotation());
		copy->SetPos(src->GetPos());

		// 順番・半径を写す
		copy->LoadData(src->SaveData());

		SceneManager::Instance().AddObject(copy);

		return copy;
	}

	//------------------------------------------------------------
	// ギミック: 同じクラスを作り、保存データ(SaveData)の内容を読み込ませて写す
	//------------------------------------------------------------
	if (auto srcGimmick = std::dynamic_pointer_cast<StageGimmick>(src))
	{
		const std::string className = srcGimmick->GetFactoryClassName();

		// 未登録のクラス名はファクトリーがassertで止まってしまうので、先に調べる
		if (!KdGameObjectFactory::Instance().GetCreateFunctions().contains(className))
		{
			KdDebugGUI::Instance().AddErrorLog("Copy failed : unknown class '%s'\n", className.c_str());
			return nullptr;
		}

		auto newObject = KdGameObjectFactory::Instance().CreateGameObject(className);

		auto copy = std::dynamic_pointer_cast<StageGimmick>(newObject);

		if (!copy)
		{
			KdDebugGUI::Instance().AddErrorLog("Copy failed : create object\n");
			return nullptr;
		}

		copy->Init();

		copy->SetFactoryClassName(className);

		// 表示名は、空いている最小の番号で付ける("Room_0"がある状態でコピーすると"Room_1")
		copy->SetObjectName(MakeUniqueObjectName(className));

		// 位置・大きさ・回転を元と同じにする
		copy->SetScale(srcGimmick->GetScale());
		copy->SetRotation(srcGimmick->GetRotation());
		copy->SetPos(srcGimmick->GetPos());

		// そのクラス固有のデータ(Roomの設定、RoomGateのLinkedRoomなど)を写す
		copy->LoadData(srcGimmick->SaveData());

		// 持ち主(追従するステージ)も元と同じにする
		copy->SetOwnerStage(srcGimmick->GetOwnerStage());

		SceneManager::Instance().AddObject(copy);

		return copy;
	}

	return nullptr;
}

std::shared_ptr<StageObject> EditorManager::FindPrimaryStage(const std::string& modelName) const
{
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto stage = std::dynamic_pointer_cast<StageObject>(obj);

		if (stage && stage->GetStageModelName() == modelName)
		{
			// 先頭から見て、最初に見つかったものが「最初のステージ」
			// (StageDataManager::SaveToFolderも同じ順番で最初の1つを保存する)
			return stage;
		}
	}

	return nullptr;
}

bool EditorManager::CanEditStageData() const
{
	auto activeStage = GetActiveStage();

	// ステージがない間は、今までどおり編集できる
	// (持ち主のないデータは、保存時に最初のステージが引き取る)
	if (!activeStage)
	{
		return true;
	}

	return FindPrimaryStage(activeStage->GetStageModelName()) == activeStage;
}

std::string EditorManager::MakeUniqueObjectName(const std::string& className) const
{
	int id = 0;

	// 0から順に、シーン上に同じ名前のオブジェクトが存在しない番号を探す
	while (true)
	{
		const std::string candidate = className + "_" + std::to_string(id);

		bool exists = false;

		for (const auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj && obj->GetObjectName() == candidate)
			{
				exists = true;
				break;
			}
		}

		if (!exists)
		{
			return candidate;
		}

		++id;
	}
}

void EditorManager::ResetState()
{
	// Play中はStartPlayModeでカーソルを隠しているので元に戻す
	// (ShowCursorは呼んだ回数分だけ増減するため、Playでない時に呼ぶと数が合わなくなる)
	if (IsPlayMode())
	{
		ShowCursor(true);
	}

	SetEditorMode(EditorMode::Edit);

	// 選択中のオブジェクトへの参照を解除
	SetSelectedObject(nullptr);

	// 編集中のステージの参照を解除
	m_wpActiveStage.reset();

	ClearDirty();

	// 編集中のステージ名などを初期状態に戻す
	m_spStageEditor = std::make_shared<StageEditor>();
}
