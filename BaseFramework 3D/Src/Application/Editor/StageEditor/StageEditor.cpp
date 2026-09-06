#include "StageEditor.h"

#include "../EditorManager.h"
#include "../../Scene/SceneManager.h"
#include"../../Scene/EditorScene/EditorScene.h"

#include "../../System/WayPointManager/WayPointManager.h"
#include"../../System/StageDataManager/StageDataManager.h"
#include"../../GameObject/Stage/Stage01/Collision/WallCollision/WallCollisionManager.h"
#include"../../GameObject/Stage/Stage01/Collision/OBBCollision/OBBCollisionManager.h"


#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"

void StageEditor::Draw()
{

	// ステージを新規作成
	if (ImGui::Button("New Stage"))
	{
		// 名前の仮登録用変数を初期化
		m_newStageName[0] = '\0';
		// ポップアップウィンドウを表示
		ImGui::OpenPopup("NewStagePopup");
	}

	ImGui::SameLine();

	// 保存しいるステージを開く
	if (ImGui::Button("Open Stage"))
	{
		// ステージリストを整理
		RefreshStageList();
		// ポップアップウィンドウを表示
		ImGui::OpenPopup("OpenStage");
	}

	ImGui::SameLine();

	// ステージを保存する
	if (ImGui::Button("Save Stage"))
	{
		// ステージ名を仮登録
		strcpy_s(m_saveStageName,sizeof(m_saveStageName),m_currentStageName.c_str());

		// ポップアップウィンドウを表示
		ImGui::OpenPopup("SaveStagePopup");
	}


	// ポップアップを表示
	DrawPopups();

}

void StageEditor::DrawPopups()
{

	// 新規作成時のPopup
	NewStagePopup();

	// 保存時のPopup
	SaveStagePopup();

	// 保存済ステージ一覧のPopup
	OpenStagePopup();

	// 保存の確認のPopup
	LoadConfirmationPopup();
	NewStageConfirmationPopup();

	// 要求があればメッセージを表示
	m_messageWindow.Draw();
}


void StageEditor::CreateNewStage()
{

	m_currentStageName = m_newStageName;

	// 既存の編集対象をクリア
	ClearStage();

	m_isEditStage = true;

	// カメラを作成(どのステージにも必ず必要)
	std::shared_ptr<TPSCamera>camera = std::make_shared<TPSCamera>();
	camera->Init();
	SceneManager::Instance().AddObject(camera);


	EditorManager::Instance().ClearDirty();

	ImGui::CloseCurrentPopup();
}

void StageEditor::SaveStagePopup()
{
	if (ImGui::BeginPopupModal("SaveStagePopup",nullptr,ImGuiWindowFlags_AlwaysAutoResize))
	{
		// ステージ名を変更可能
		ImGui::InputText("Stage Name",m_saveStageName,sizeof(m_saveStageName));


		std::string stageName = m_saveStageName;
		// 保存するステージ名が変更されたかどうか
		bool stageNameChange = m_saveStageName != m_currentStageName;

		if (ImGui::Button("Save")&&!stageName.empty())
		{
			// もし編集対象のステージ無ければセーブしない
			if (!m_isEditStage)
			{
				m_messageWindow.Open(U8("エラー"), U8("編集中のステージがないためセーブできません"));
				ImGui::CloseCurrentPopup();
			}
			// 既に存在するかつ、保存するステージ名が変更されたら
			else if (IsStageExists(stageName) && stageNameChange)
			{
				ImGui::OpenPopup("OverwritePopup");
			}
			else
			{
				if (StageDataManager::Instance().Save(stageName))
				{
					m_currentStageName = stageName;

					EditorManager::Instance().ClearDirty();

					m_hasSaveTarget = true;

					ImGui::CloseCurrentPopup();
				}
				else
				{
					m_messageWindow.Open(U8("エラー"),U8("ステージのセーブに失敗しました"));
				}
			}

		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		// 上書き確認Popup
		OverwritePopup();

		// OverwritePopupでYesが押された
		if (m_closeSavePopup)
		{
			m_closeSavePopup = false;

			// SavePopupを閉じる
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void StageEditor::OverwritePopup()
{
	if (ImGui::BeginPopupModal("OverwritePopup",nullptr,ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(U8("Stage '%s' が既に保存されています"),m_saveStageName);

		ImGui::Text(U8("上書きしますか？"));

		if (ImGui::Button("Yes"))
		{
			const std::string stageName = m_saveStageName;

			if (StageDataManager::Instance().Save(stageName))
			{
				m_currentStageName = stageName;

				EditorManager::Instance().ClearDirty();

				m_hasSaveTarget = true;

				// OverwritePopupを閉じる
				ImGui::CloseCurrentPopup();

				// 親のSaveStagePopupも閉じる
				m_closeSavePopup = true;
			}
			else
			{
				m_messageWindow.Open(U8("エラー"),U8("ステージのセーブに失敗しました"));
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("No"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void StageEditor::RefreshStageList()
{
	m_stageNames.clear();

	const std::filesystem::path stageRoot = "Asset/Data/Stage";

	if (!std::filesystem::exists(stageRoot))
	{
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(stageRoot))
	{
		if (!entry.is_directory())
		{
			continue;
		}

		const auto stageDataPath = entry.path() / "StageData.json";

		// StageData.jsonが存在するフォルダーだけ表示する
		if (!std::filesystem::exists(stageDataPath))
		{
			continue;
		}
		m_stageNames.push_back(entry.path().filename().string());
	}

	// ファイルシステムの列挙順に依存しないよう並べ替える
	std::sort(m_stageNames.begin(), m_stageNames.end());

}
void StageEditor::OpenStagePopup()
{
	if (ImGui::BeginPopupModal("OpenStage", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Select Stage");

		ImGui::Separator();

		if (m_stageNames.empty())
		{
			ImGui::TextDisabled("No saved stages were found.");
		}


		// 残り領域をスクロール可能なChildとして使う
		if (ImGui::BeginChild("StageList", ImVec2(0, 100), true))
		{
			// ステージ一覧を表示
			for (const auto& stageName : m_stageNames)
			{
				const bool isCurrentStage = stageName == m_currentStageName;

				ImGui::PushID(stageName.c_str());

				if (ImGui::Selectable(stageName.c_str(), isCurrentStage))
				{
					RequestLoadStage(stageName);
				}

				ImGui::PopID();
			}
		}
		ImGui::EndChild();

		ImGui::Separator();

		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		if (m_closeOpenStagePopup)
		{
			m_closeOpenStagePopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

}
void StageEditor::RequestLoadStage(const std::string & stageName)
{
	if (stageName.empty())
	{
		return;
	}

	// 現在開いているステージを再度選んだ場合
	if (stageName == m_currentStageName)
	{
		return;
	}

	m_pendingLoadStageName = stageName;

	// 現在のステージがなければ、そのままロードする
	if (m_currentStageName.empty())
	{
		ClearStage();

		if (StageDataManager::Instance().Load(m_pendingLoadStageName))
		{
			// ロード成功後の処理
			LoadSucceeded(m_pendingLoadStageName);
		}
		else
		{
			m_messageWindow.Open(U8("エラー"), U8("ファイルを開けません"));
			LoadFailed();
		}
		return;
	}

	// 編集したステージが保存されていなければ確認する
	if(EditorManager::Instance().IsDirty())
	{
		// Popup描画の外側で保存確認を開くための要求
		m_requestLoadConfirmation = true;
	}
	else
	{
		ClearStage();

		if (StageDataManager::Instance().Load(m_pendingLoadStageName))
		{
			// ロード成功後の処理
			LoadSucceeded(m_pendingLoadStageName);
		}
		else
		{
			LoadFailed();
		}
		return;
	}

}
void StageEditor::LoadConfirmationPopup()
{
	// 新しいステージを開く前に保存をするか確認する
	if (m_requestLoadConfirmation)
	{
		ImGui::OpenPopup("LoadConfirmationPopup");
		m_requestLoadConfirmation = false;
	}

	if (ImGui::BeginPopupModal("LoadConfirmationPopup",nullptr,ImGuiWindowFlags_AlwaysAutoResize))
	{

		ImGui::Text(U8("現在編集中のステージを保存しますか？"));
		ImGui::Text("Current : %s",m_currentStageName.c_str());
		ImGui::Text("Load    : %s",m_pendingLoadStageName.c_str());

		ImGui::Separator();

		if (ImGui::Button("Save and Load"))
		{
			// 現在のステージを保存できた場合だけロードする
			if (StageDataManager::Instance().Save(m_currentStageName))
			{
				ClearStage();

				if (StageDataManager::Instance().Load(m_pendingLoadStageName))
				{
					// ロード成功後の処理
					LoadSucceeded(m_pendingLoadStageName);

					ImGui::CloseCurrentPopup();
				}
				else
				{
					LoadFailed();
				}
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Don't Save"))
		{
			ClearStage();

			// 新しく開くステージ名
			if (StageDataManager::Instance().Load(m_pendingLoadStageName))
			{
				// ロード成功後の処理
				LoadSucceeded(m_pendingLoadStageName);
				ImGui::CloseCurrentPopup();
			}
			else
			{
				LoadFailed();
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			m_pendingLoadStageName.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
void StageEditor::LoadSucceeded(const std::string& stageName)
{
	// ロードに成功したのでバックアップをクリア
	auto editorScene = SceneManager::Instance().GetCurrentScene<EditorScene>();
	if (!editorScene)
	{
		return;
	}

	editorScene->ClearBackupList();
	WayPointManager::Instance().ClearBackup();
	WallCollisionManager::Instance().ClearBackup();

	OBBCollisionManager::Instance().ClearBackup();

	EditorManager::Instance().ClearDirty();

	m_currentStageName = stageName;

	m_pendingLoadStageName.clear();
	m_closeOpenStagePopup = true;

	m_isEditStage = true;

	m_hasSaveTarget = true;

}

void StageEditor::LoadFailed()
{
	// ロードに失敗した場合は復元
	auto editorScene = SceneManager::Instance().GetCurrentScene<EditorScene>();
	if (!editorScene)
	{
		return;
	}

	editorScene->RestoreObjectList();;

	// ウェイポイントを復元
	WayPointManager::Instance().RestoreWayPoints();

	WallCollisionManager::Instance().RestoreWallCollisionList();

	OBBCollisionManager::Instance().RestoreOBBCollisionList();
}

bool StageEditor::IsStageExists(const std::string& stageName) const
{
	const std::filesystem::path stageFolder =
		std::filesystem::path("Asset/Data/Stage")
		/ stageName;

	const auto stageDataPath =
		stageFolder / "StageData.json";


	return std::filesystem::exists(stageDataPath);
}

void StageEditor::NewStagePopup()
{
	if (ImGui::BeginPopupModal("NewStagePopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// ステージ名を入力
		ImGui::Text("Stage Name");
		ImGui::InputText("##StageName", m_newStageName, sizeof(m_newStageName));

		// 新規作成するステージ名
		std::string newStageName = m_newStageName;

		if (ImGui::Button("Create")&& !newStageName.empty())
		{

			// 変更後セーブをしていなかったら
			if (EditorManager::Instance().IsDirty())
			{
				m_requestNewStageConfirmation = true;
			}
			else
			{
				CreateNewStage();
			}
		}
	
		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		if (EditorManager::Instance().IsDirty())
		{
			SaveStagePopup();
		}


		ImGui::EndPopup();
	}

}

void StageEditor::NewStageConfirmationPopup()
{
	if (m_requestNewStageConfirmation)
	{
		ImGui::OpenPopup("NewStageConfirmationPopup");
		m_requestNewStageConfirmation = false;
	}

	if (ImGui::BeginPopupModal("NewStageConfirmationPopup",nullptr,ImGuiWindowFlags_AlwaysAutoResize))
	{

		ImGui::Text(U8("現在編集中のステージに未保存の変更があります。"));
		ImGui::Text(U8("新しいステージを作成する前に保存しますか？"));

		ImGui::Separator();

		// 保存してからNew Stage
		if (ImGui::Button("Save"))
		{
			// 保存するステージ名
			std::string saveStageName = m_currentStageName;

			// 保存済のステージと被っていたら
			if (IsStageExists(saveStageName)&&!m_hasSaveTarget)
			{
				m_messageWindow.Open(
					U8("ステージ名エラー"),
					U8("同じ名前のステージが既に存在します。\n別のステージ名を入力して下さい"));

				ImGui::CloseCurrentPopup();
			}
			else if (StageDataManager::Instance().Save(m_currentStageName))
			{

				CreateNewStage();
				ImGui::CloseCurrentPopup();
			}
			else
			{
				m_messageWindow.Open(
					U8("エラー"),
					U8("ステージのセーブに失敗しました"));
			}
		}

		ImGui::SameLine();

		// 保存せずNew Stage
		if (ImGui::Button("Don't Save"))
		{
			CreateNewStage();

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		// New Stage自体を中止
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void StageEditor::ClearStage()
{
	// 選択中のオブジェクトへの参照を先に解除する
	EditorManager::Instance().SetSelectedObject(nullptr);

	// WayPointをクリア
	WayPointManager::Instance().ClearWayPoints();

	// ABBをクリア
	WallCollisionManager::Instance().ClearWallCollisionList();

	// OBBをクリア
	OBBCollisionManager::Instance().ClearOBBCollisionList();

	// ゲームオブジェクトをクリア
	auto editorScene = SceneManager::Instance().GetCurrentScene<EditorScene>();
	if (!editorScene)
	{
		return;
	}
	editorScene->BackupObjectList();
}
