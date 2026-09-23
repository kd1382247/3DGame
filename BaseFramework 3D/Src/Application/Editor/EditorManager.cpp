#include "EditorManager.h"

#include"../../Framework/Effekseer/KdEffekseerManager.h"
#include"../GameObject/Camera/CameraBase.h"
#include "../../Framework/GameObject/KdGameObjectFactory.h"
#include"../System/WayPointManager/WayPointManager.h"
#include"../GameObject/WayPoint/WayPoint.h"
#include"../GameObject/Stage/Collision/AABBCollision/AABBCollisionManager.h"
#include"../GameObject/Stage/Collision/AABBCollision/AABBCollision.h"

#include"../GameObject/Stage/Collision/OBBCollision/OBBCollisionManager.h"
#include"../GameObject/Stage/Collision/OBBCollision/OBBCollision.h"

#include"../Scene/EditorScene/EditorScene.h"

#include"../System/ReferenceManager/ReferenceManager.h"
#include "../Scene/SceneManager.h"
#include"../System/StageDataManager/StageDataManager.h"

#include "Hierarchy/Hierarchy.h"
#include "Inspector/Inspector.h"
#include "StageEditor/StageEditor.h"
#include"SceneView/SceneView.h"

void EditorManager::Init()
{
	if(!m_spHierarchy)
	{
		m_spHierarchy = std::make_shared<Hierarchy>();
	}

	if (!m_spInspector)
	{
		m_spInspector = std::make_shared<Inspector>();
	}

	if (!m_spStageEditor)
	{
		m_spStageEditor = std::make_shared<StageEditor>();
	}

	if (!m_spMessageWindow)
	{
		m_spMessageWindow = std::make_shared<MessageWindow>();
	}

	if (!m_spSceneView)
	{
		m_spSceneView = std::make_shared<SceneView>();
		m_spSceneView->Init();
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
		m_spHierarchy->Draw();
		m_spInspector->Draw();
		m_spSceneView->Draw();

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
	editorScene->BackupObjectList();
	WayPointManager::Instance().ClearWayPoints();
	AABBCollisionManager::Instance().ClearAABBCollisionList();
	OBBCollisionManager::Instance().ClearOBBCollisionList();


	// Edit開始前の状態を復元
	if (!StageDataManager::Instance().LoadTemporary())
	{
		// ロードに失敗したため復元する
		editorScene->RestoreObjectList();
		WayPointManager::Instance().RestoreWayPoints();
		AABBCollisionManager::Instance().RestoreAABBCollisionList();
		OBBCollisionManager::Instance().RestoreOBBCollisionList();

		return;
	}

	// バックアップリストをクリア
	editorScene->ClearBackupList();
	WayPointManager::Instance().ClearBackup();
	AABBCollisionManager::Instance().ClearBackup();
	OBBCollisionManager::Instance().ClearBackup();

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

	auto editorScene = SceneManager::Instance().GetCurrentScene<EditorScene>();
	if (!editorScene)
	{
		return;
	}

	// Play中の状態を削除
	editorScene->BackupObjectList();
	WayPointManager::Instance().ClearWayPoints();
	AABBCollisionManager::Instance().ClearAABBCollisionList();
	OBBCollisionManager::Instance().ClearOBBCollisionList();

	// Edit開始前の状態を復元
	if (!StageDataManager::Instance().LoadTemporary())
	{
		// ロードに失敗したため復元する
		editorScene->RestoreObjectList();
		WayPointManager::Instance().RestoreWayPoints();
		AABBCollisionManager::Instance().RestoreAABBCollisionList();
		OBBCollisionManager::Instance().RestoreOBBCollisionList();

		return;
	}

	// バックアップリストをクリア
	editorScene->ClearBackupList();
	WayPointManager::Instance().ClearBackup();
	AABBCollisionManager::Instance().ClearBackup();
	OBBCollisionManager::Instance().ClearBackup();

	// モードを切り替える
	SetEditorMode(EditorMode::Edit);

	// エフェクトをストップ
	KdEffekseerManager::GetInstance().StopAllEffect();

	// カラースフィアを消す
	KdShaderManager::Instance().ClearColorSphere();

	// マウスカーソル
	ShowCursor(true);
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
		ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::Begin("Menu",nullptr,flags);

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

KdCollider::RayInfo EditorManager::CreateRayInfo(KdCollider::Type type)
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
	case Hierarchy::HierarchyCategory::GameObject :

		SelectGameObjectByMouse();
		break;

	case Hierarchy::HierarchyCategory::Stage:
		SelectStageObjectByMouse();
		break;

	case Hierarchy::HierarchyCategory::WayPoint:
		SelectWayPointByMouse();
		break;

	case Hierarchy::HierarchyCategory::CollisionBox:
		SelectBoxByMouse();
		break;
	case Hierarchy::HierarchyCategory::OBB:
		SelectOBBByMouse();
		break;

	}
}

template<typename Container, typename Predicate>
std::shared_ptr<KdGameObject> EditorManager::SelectClosestByMouse(const Container& objects, KdCollider::Type rayType, Predicate predicate)
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
			return obj->GetObjectCategory() == KdGameObject::ObjectCategory::Stage ||
				   obj->GetObjectCategory() == KdGameObject::ObjectCategory::Gimmick;
		});

	SetSelectedObject(selectedObj);
}

void EditorManager::SelectWayPointByMouse()
{
	auto selectedObj = SelectClosestByMouse(
		WayPointManager::Instance().GetWayPoints(),
		KdCollider::TypeBump,
		[](const std::shared_ptr<KdGameObject>&) { return true; });

	SetSelectedObject(selectedObj);
}

void EditorManager::SelectBoxByMouse()
{
	auto selectedObj = SelectClosestByMouse(
		AABBCollisionManager::Instance().GetAABBCollisionList(),
		KdCollider::TypeBump,
		[](const std::shared_ptr<KdGameObject>&) { return true; });

	SetSelectedObject(selectedObj);
}

void EditorManager::SelectOBBByMouse()
{
	auto selectedObj = SelectClosestByMouse(
		OBBCollisionManager::Instance().GetOBBCollisionList(),
		KdCollider::TypeBump,
		[](const std::shared_ptr<KdGameObject>&) { return true; });

	SetSelectedObject(selectedObj);
}

void EditorManager::CreateGameObject(const std::string& className)
{
	auto newObject =
		KdGameObjectFactory::Instance().CreateGameObject(className);

	if (!newObject)
	{
		OutputDebugStringA("オブジェクトの生成に失敗しました\n");
		KdDebugGUI::Instance().AddErrorLog("オブジェクトの生成に失敗しました\n");
		return;
	}
	newObject->Init();

	// ファクトリー登録名を保持しておく(SaveDataの"Class"に使う。表示名とは別)
	newObject->SetFactoryClassName(className);

	// 表示名は"Slime_0"のように自動採番する
	newObject->SetObjectName(MakeUniqueObjectName(className));

	SceneManager::Instance().AddObject(newObject);

	// 現在選択中のオブジェクト
	SetSelectedObject(newObject);
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
