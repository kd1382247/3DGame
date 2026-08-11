#pragma once

#include "Hierarchy/Hierarchy.h"
#include "Inspector/Inspector.h"
#include "StageEditor/StageEditor.h"

class EditorManager
{
public:
	enum class EditorMode
	{
		Edit,
		Play
	};

	void Draw();

	void SetEditorMode(EditorMode mode) { m_editorMode = mode; }
	EditorMode GetEditorMode() const { return m_editorMode; }

	// エディタの状態
	bool IsEditMode() const { return m_editorMode == EditorMode::Edit; }
	bool IsPlayMode() const { return m_editorMode == EditorMode::Play; }

	// 現在選択されているオブジェクト
	void SetSelectedObject(const std::shared_ptr<KdGameObject>& obj)
	{
		m_spSelectedObject = obj;
	}

	void CreateGameObject(const std::string& className);

	const std::shared_ptr<KdGameObject>& GetSelectedObject() const
	{
		return m_spSelectedObject;
	}

private:
	void DrawModeMenu();

	std::shared_ptr<KdGameObject> m_spSelectedObject;

	Hierarchy m_hierarchy;
	Inspector m_inspector;
	StageEditor m_stageEditor;
	EditorMode m_editorMode = EditorMode::Edit;

private: // シングルトンパターン
	EditorManager() = default;
	~EditorManager() = default;

public:

	static EditorManager& Instance()
	{
		static EditorManager instance;
		return instance;
	}

};
