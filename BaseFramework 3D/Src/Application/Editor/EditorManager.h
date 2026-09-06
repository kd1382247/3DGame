#pragma once


class Hierarchy;
class Inspector;
class StageEditor;
class SceneView;

class CameraBase;
class MessageWindow;

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

	// レンダーターゲットを切り替え
	void BeginSceneViewRender();
	void EndSceneViewRender();


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

	void Init();

	// メニュー表示
	void DrawToolBar();


	// ステージ名の長さを調べて返す
	std::string MakeEllipsisText(const std::string& text, float maxWidth);

	// レイ情報を生成
	KdCollider::RayInfo CreateRayInfo(KdCollider::Type type);

	// SceneViewのアスペクト比のに合わせて更新
	void UpdateSceneViewProjection();

	// マウスクリックでオブジェクトを選択
	void UpdateMouseSelection();

	// キャラクターを選択
	void SelectGameObjectByMouse();

	// ステージを選択
	void SelectStageObjectByMouse();

	// ウェイポイントを選択
	void SelectWayPointByMouse();

	// AABBを選択
	void SelectBoxByMouse();

	// OBBを選択
	void SelectOBBByMouse();


private:

	std::shared_ptr<KdGameObject> m_spSelectedObject=nullptr;

	std::shared_ptr<Hierarchy> m_spHierarchy=nullptr;
	std::shared_ptr<Inspector> m_spInspector=nullptr;
	std::shared_ptr<StageEditor> m_spStageEditor=nullptr;
	std::shared_ptr<SceneView>m_spSceneView = nullptr;


	// エディタのモード
	EditorMode m_editorMode = EditorMode::Edit;

	// エディターカメラ
	std::weak_ptr<CameraBase>m_wpEditorCamera;

	// メッセージウィンドウ
	std::shared_ptr<MessageWindow>m_spMessageWindow = nullptr;

	// 編集したかどうか
	bool m_isDirty = false;


private: // シングルトンパターン
	EditorManager() { Init(); }
	~EditorManager() {}

public:

	static EditorManager& Instance()
	{
		static EditorManager instance;
		return instance;
	}

};
