#include "StageEditor.h"

#include"../EditorManager.h"
#include"../../Scene/SceneManager.h"
#include"../../System/PathFinding/WayPointManager.h"

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
		ImGui::OpenPopup("NewStagePopup");

	}

	ImGui::SameLine();

	if (ImGui::Button("Open Stage"))
	{
		LoadStage("Stage02");
	}

	ImGui::SameLine();

	if (ImGui::Button("Save Stage"))
	{
	
		RequestSaveStage();
	}

	DrawNewStagePopup();

	DrawSaveStagePopup();
	

}

void StageEditor::CreateNewStage()
{
	std::string stageName = m_newStageName;

	// 空文字なら作らない
	if (stageName.empty())
	{
		return;
	}

	m_currentStageName = stageName;

	// 既存の編集対象をクリア
	ClearStage();


	ImGui::CloseCurrentPopup();
}

bool StageEditor::SaveStage()
{

	if (m_currentStageName.empty())
	{
		return false;
	}

	// フォルダー作成
	std::filesystem::path stageFolder =
		std::filesystem::path("Asset/Data/Stage") / m_currentStageName;

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
		stageJson["Objects"].push_back( obj->SaveData());
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
					// 新しい名前で保存
					m_currentStageName = stageName;

					SaveStage();

					ImGui::CloseCurrentPopup();
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
			m_currentStageName =
				m_saveStageName;

			if (SaveStage())
			{
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

	ClearStage();

	// 指定のフォルダーパス
	std::filesystem::path stageFolder =std::filesystem::path("Asset/Data/Stage") / stageName;

	// ステージデータ
	std::filesystem::path stageDataPath =stageFolder / "StageData.json";
	// ウェイポイントデータ
	std::filesystem::path wayPointDataPath =stageFolder / "WayPointData.json";


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

	if (!stageJson.contains("Objects") ||!stageJson["Objects"].is_array())
	{
		return false;
	}

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

		std::string className = objectJson["Class"].get<std::string>();
		auto obj = KdGameObjectFactory::Instance().CreateGameObject(className);

		if (!obj)
		{
			continue;
		}

		obj->Init();
		// 名前読込
		std::string name = objectJson["Name"];
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
void StageEditor::DrawNewStagePopup()
{
	if (ImGui::BeginPopupModal("NewStagePopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{

		ImGui::Text("Stage Name");


		if (ImGui::InputText("##StageName", m_newStageName, sizeof(m_newStageName)));


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

	// 選択中のオブジェクトを空にする
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

		if (obj->GetObjectName() == "EditorCamera")
		{
			continue;
		}

		obj->Destroy();
	}
}
