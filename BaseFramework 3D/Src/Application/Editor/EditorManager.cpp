#include "EditorManager.h"

#include "../../Framework/GameObject/KdGameObjectFactory.h"

#include"../System/ReferenceManager/ReferenceManager.h"
#include "../Scene/SceneManager.h"


void EditorManager::Draw()
{
	ImGui::Begin("Menu");

	m_stageEditor.Draw();

	DrawModeMenu();

	ImGui::End();

	m_hierarchy.Draw();
	m_inspector.Draw();
}

void EditorManager::DrawModeMenu()
{
	if (IsEditMode())
	{
		ImGui::Text("CurrentMode: Edit");
	}
	else
	{
		ImGui::Text("CurrentMode: Play");
	}

	if (ImGui::Button("Edit"))
	{
		SetEditorMode(EditorMode::Edit);
	}

	ImGui::SameLine();

	if (ImGui::Button("Play"))
	{
		SetEditorMode(EditorMode::Play);

		// 全てのオブジェクトを生成後に関連付け
		ReferenceManager::Instance().AssociateObjects(SceneManager::Instance().GetObjList());
	}
}

void EditorManager::CreateGameObject(const std::string& className)
{
	auto newObject =
		KdGameObjectFactory::Instance().CreateGameObject(className);

	if (!newObject)
	{
		OutputDebugStringA(
			"オブジェクトの生成に失敗しました\n");
		return;
	}
	newObject->Init();

	SceneManager::Instance().AddObject(newObject);

	// 現在選択中のオブジェクト
	SetSelectedObject(newObject);
}
