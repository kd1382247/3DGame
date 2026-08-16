#pragma once

#include "Hierarchy/Hierarchy.h"
#include "Inspector/Inspector.h"
#include "StageEditor/StageEditor.h"

class CameraBase;

class EditorManager
{
public:
	enum class EditorMode
	{
		Edit,
		Play
	};

	void Draw();


	void StartPlayMode();
	void StopPlayMode();


	// エディタのモードを変更する
	void SetEditorMode(EditorMode mode) { m_editorMode = mode; }
	
	// エディタの状態を返す
	bool IsEditMode() const { return m_editorMode == EditorMode::Edit; }
	bool IsPlayMode() const { return m_editorMode == EditorMode::Play; }

	// 現在選択されているオブジェクト
	void SetSelectedObject(const std::shared_ptr<KdGameObject>& obj)
	{
		m_spSelectedObject = obj;
	}

	const std::shared_ptr<KdGameObject>& GetSelectedObject() const
	{
		return m_spSelectedObject;
	}

	// オブジェクトを作成
	void CreateGameObject(const std::string& className);

	
	// 編集したかどうか
	const bool IsDirty()const { return m_isDirty; }
	void ClearDirty() { m_isDirty = false; }
	void MarkDirty() { m_isDirty = true; }

private:

	// メニュー表示
	void DrawModeMenu();

	std::shared_ptr<KdGameObject> m_spSelectedObject;

	Hierarchy m_hierarchy;
	Inspector m_inspector;
	StageEditor m_stageEditor;
	EditorMode m_editorMode = EditorMode::Edit;

	std::weak_ptr<CameraBase>m_wpCamera;

	// 編集したかどうか
	bool m_isDirty = false;


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
