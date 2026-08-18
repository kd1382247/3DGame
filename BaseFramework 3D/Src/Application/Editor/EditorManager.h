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

	void SetEditorCamera(std::shared_ptr<CameraBase>camera) { m_wpEditorCamera = camera; }

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

		// 前に選択されていたフラグをfalse
		if (m_spSelectedObject){m_spSelectedObject->SetSelected(false);}

		m_spSelectedObject = obj;

		// 新たに選択されたフラグをtrueにする
		if (m_spSelectedObject){m_spSelectedObject->SetSelected(true);}
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
	void DrawToolBar();

	// ステージ名の長さを調べて返す
	std::string MakeEllipsisText(const std::string& text, float maxWidth);

	// レイ情報を生成
	KdCollider::RayInfo CreateReyInfo(KdCollider::Type type);

	// マウスクリックでオブジェクトを選択
	void UpdateMouseSelection();

	// キャラクターを選択
	void SelectGameObjectByMouse();

	// ステージを選択
	void SelectStageObjectByMouse();

	// ウェイポイントを選択
	void SelectWayPointByMouse();

private:

	std::shared_ptr<KdGameObject> m_spSelectedObject;

	Hierarchy m_hierarchy;
	Inspector m_inspector;
	StageEditor m_stageEditor;
	EditorMode m_editorMode = EditorMode::Edit;

	std::weak_ptr<CameraBase>m_wpTPSCamera;

	std::weak_ptr<CameraBase>m_wpEditorCamera;

	

	// 編集したかどうか
	bool m_isDirty = false;


private: // シングルトンパターン
	EditorManager() {}
	~EditorManager() {}

public:

	static EditorManager& Instance()
	{
		static EditorManager instance;
		return instance;
	}

};
