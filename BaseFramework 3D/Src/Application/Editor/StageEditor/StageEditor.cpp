#include "StageEditor.h"

#include "../EditorManager.h"
#include "../../Scene/SceneManager.h"
#include "../../System/PathFinding/WayPointManager.h"
#include"../../System/ReferenceManager/ReferenceManager.h"

void StageEditor::Draw()
{
	if (m_currentStageName.empty())
	{
		ImGui::Text("Current Stage : None");
	}
	else
	{
		ImGui::Text("Current Stage : %s", m_currentStageName.c_str());
	}


	if (ImGui::Button("New Stage"))
	{
		m_newStageName[0] = '\0';
		ImGui::OpenPopup("NewStagePopup");
	}

	ImGui::SameLine();

	if (ImGui::Button("Open Stage"))
	{
		RefreshStageList();
		ImGui::OpenPopup("OpenStagePopup");
	}

	ImGui::SameLine();

	if (ImGui::Button("Save Stage"))
	{
		RequestSaveStage();
	}

	// 新規作成時のPopup
	DrawNewStagePopup();
	// 保存時のPopup
	DrawSaveStagePopup();

	// 保存済ステージ一覧のPopup
	DrawOpenStagePopup();

	DrawLoadConfirmationPopup();
}

void StageEditor::CreateNewStage()
{
	std::string stageName = m_newStageName;

	// 空文字なら作らない
	if (stageName.empty())
	{
		return;
	}

	// 既存の編集対象をクリア
	ClearStage();

	m_currentStageName = stageName;

	ImGui::CloseCurrentPopup();
}

bool StageEditor::SaveStage(const std::string& stageName)
{
	if (stageName.empty())
	{
		return false;
	}

	const std::filesystem::path stageFolder = GetStageFolder(stageName);

	// フォルダーがない場合作成
	std::filesystem::create_directories(stageFolder);

	// StageDataを作成
	nlohmann::json stageJson;

	stageJson["Objects"] = nlohmann::json::array();

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		if (!obj)
		{
			continue;
		}

		if (obj->GetObjectCategory() == KdGameObject::ObjectCategory::None)
		{
			continue;
		}

		// Jsonファイルに追加
		stageJson["Objects"].push_back(obj->SaveData());
	}

	// ファイル作成
	std::ofstream file(stageFolder / "StageData.json");

	if (!file.is_open())
	{
		return false;
	}

	file << stageJson.dump(4);

	// WayPointを保存
	if (!WayPointManager::Instance().Save((stageFolder / "WayPointData.json").string()))
	{
		return false;
	}

	return true;
}

void StageEditor::RequestSaveStage()
{
	strcpy_s(
		m_saveStageName,
		sizeof(m_saveStageName),
		m_currentStageName.c_str()
	);

	ImGui::OpenPopup("SaveStagePopup");
}

void StageEditor::DrawSaveStagePopup()
{
	if (ImGui::BeginPopupModal(
		"SaveStagePopup",
		nullptr,
		ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText(
			"Stage Name",
			m_saveStageName,
			sizeof(m_saveStageName)
		);

		if (ImGui::Button("Save"))
		{
			std::string stageName = m_saveStageName;

			if (!stageName.empty())
			{
				std::filesystem::path stageFolder =
					std::filesystem::path("Asset/Data/Stage")
					/ stageName;

				// 既に存在する
				if (std::filesystem::exists(stageFolder))
				{
					ImGui::OpenPopup("OverwritePopup");
				}
				else
				{
					if (SaveStage(stageName))
					{
						m_currentStageName = stageName;
						ImGui::CloseCurrentPopup();
					}
				}
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		// 上書き確認Popup
		DrawOverwritePopup();

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

void StageEditor::DrawOverwritePopup()
{
	if (ImGui::BeginPopupModal(
		"OverwritePopup",
		nullptr,
		ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(
			"Stage '%s' already exists.",
			m_saveStageName
		);

		ImGui::Text("Overwrite?");

		if (ImGui::Button("Yes"))
		{
			const std::string stageName = m_saveStageName;

			if (SaveStage(stageName))
			{
				m_currentStageName = stageName;
				// OverwritePopupを閉じる
				ImGui::CloseCurrentPopup();

				// 親のSaveStagePopupも閉じる
				m_closeSavePopup = true;
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
bool StageEditor::LoadStage(const std::string& stageName)
{
	const std::filesystem::path stageFolder = GetStageFolder(stageName);

	// ステージデータ
	const std::filesystem::path stageDataPath = stageFolder / "StageData.json";
	// ウェイポイントデータ
	const std::filesystem::path wayPointDataPath = stageFolder / "WayPointData.json";

	// 読込失敗で現在の編集内容を消さないよう、先に必要ファイルを確認する
	if (!std::filesystem::exists(stageDataPath) ||
		!std::filesystem::exists(wayPointDataPath))
	{
		return false;
	}


	std::ifstream file(stageDataPath);

	if (!file.is_open())
	{
		return false;
	}

	nlohmann::json stageJson;
	try
	{
		file >> stageJson;
	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");

		return false;
	}

	if (!stageJson.contains("Objects") || !stageJson["Objects"].is_array())
	{
		return false;
	}

	// ファイルとJSON形式を確認できてから現在の編集対象を消す
	ClearStage();

	// オブジェクトを生成
	for (const auto& objectJson : stageJson["Objects"])
	{

		if (!objectJson.contains("Class") ||
			!objectJson.contains("Name") ||
			!objectJson.contains("Position") ||
			!objectJson.contains("Scale") ||
			!objectJson.contains("Rotation"))
		{
			OutputDebugStringA("必要なデータが不足しているオブジェクトをスキップしました\n");
			continue;
		}

		const std::string className = objectJson["Class"].get<std::string>();
		auto obj = KdGameObjectFactory::Instance().CreateGameObject(className);

		if (!obj)
		{
			continue;
		}

		obj->Init();
		// 名前読込
		const std::string name = objectJson["Name"];
		obj->SetObjectName(name);

		// 座標読込
		Math::Vector3 pos;
		pos.x = objectJson["Position"]["x"].get<float>();
		pos.y = objectJson["Position"]["y"].get<float>();
		pos.z = objectJson["Position"]["z"].get<float>();

		obj->SetPos(pos);

		// 大きさ読込
		Math::Vector3 scale;
		scale.x = objectJson["Scale"]["x"].get<float>();
		scale.y = objectJson["Scale"]["y"].get<float>();
		scale.z = objectJson["Scale"]["z"].get<float>();

		obj->SetScale(scale);

		// 回転
		Math::Vector3 rotation;
		rotation.x = objectJson["Rotation"]["x"].get<float>();
		rotation.y = objectJson["Rotation"]["y"].get<float>();
		rotation.z = objectJson["Rotation"]["z"].get<float>();

		obj->SetRotation(rotation);

		SceneManager::Instance().AddObject(obj);
	}

	// ウェイポイントを生成
	if (!WayPointManager::Instance().Load(wayPointDataPath.string()))
	{
		return false;
	}

	
	m_currentStageName = stageName;

	return true;
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
void StageEditor::DrawOpenStagePopup()
{
	if (ImGui::BeginPopupModal(
		"OpenStagePopup", 
		nullptr, 
		ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Select Stage");

		ImGui::Separator();

		if (m_stageNames.empty())
		{
			ImGui::TextDisabled("No saved stages were found.");
		}

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

		ImGui::Separator();

		if (ImGui::Button("Refresh"))
		{
			RefreshStageList();
		}

		ImGui::SameLine();

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
		if (LoadStage(m_pendingLoadStageName))
		{
			m_pendingLoadStageName.clear();
			m_closeOpenStagePopup = true;
		}

		return;
	}

	// Popup描画の外側で保存確認を開くための要求
	m_requestLoadConfirmation = true;
}


void StageEditor::DrawLoadConfirmationPopup()
{
	if (m_requestLoadConfirmation)
	{
		ImGui::OpenPopup("LoadConfirmationPopup");
		m_requestLoadConfirmation = false;
	}

	if (ImGui::BeginPopupModal(
		"LoadConfirmationPopup",
		nullptr,
		ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(
			"Save the current stage before loading?");

		ImGui::Text(
			"Current : %s",
			m_currentStageName.c_str());

		ImGui::Text(
			"Load    : %s",
			m_pendingLoadStageName.c_str());

		ImGui::Separator();

		if (ImGui::Button("Save and Load"))
		{
			// 現在のステージを保存できた場合だけロードする
			if (SaveStage(m_currentStageName))
			{
				if (LoadStage(m_pendingLoadStageName))
				{
					m_pendingLoadStageName.clear();
					m_closeOpenStagePopup = true;

					ImGui::CloseCurrentPopup();
				}
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Don't Save"))
		{
			if (LoadStage(m_pendingLoadStageName))
			{
				m_pendingLoadStageName.clear();
				m_closeOpenStagePopup = true;

				ImGui::CloseCurrentPopup();
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
void StageEditor::DrawNewStagePopup()
{
	if (ImGui::BeginPopupModal("NewStagePopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{

		ImGui::Text("Stage Name");


		ImGui::InputText("##StageName", m_newStageName, sizeof(m_newStageName));


		if (ImGui::Button("Create"))
		{
			CreateNewStage();
		}

		ImGui::SameLine();

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
	WayPointManager::Instance().Clear();

	// ゲームオブジェクトをクリア
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		if (!obj)
		{
			continue;
		}

		// Category::NoneはEditorCameraなど、ステージ保存対象外のオブジェクト
		if (obj->GetObjectCategory() == KdGameObject::ObjectCategory::None||
			obj->GetObjectCategory()==KdGameObject::ObjectCategory::Camera)
		{
			continue;
		}

		obj->Destroy();
	}
}

std::filesystem::path StageEditor::GetStageFolder(const std::string& stageName) const
{
	return std::filesystem::path("Asset/Data/Stage") / stageName;
}
