#pragma once

class CameraBase;

class BaseScene
{
public :

	BaseScene()			 { Init(); }
	virtual ~BaseScene() {}

	void PreUpdate();
	void Update();
	void PostUpdate();

	virtual void PreDraw();
	void Draw();
	void DrawSprite();
	virtual void DrawDebug();

	virtual bool UsePostProcess()const
	{
		return true;
	}

	virtual void EditorUpdate();

	// オブジェクトリストを取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList()
	{
		return m_objList;
	}
	
	// オブジェクトリストに追加
	void AddObject(const std::shared_ptr<KdGameObject>& _obj)
	{
		m_objList.push_back(_obj);
	}

protected :

	// このシーンでEdit/Play切り替えを使うか
	virtual bool UseEditorMode()const
	{
		return false;
	}

	bool IsEditing()const;

	// 継承先シーンで必要ならオーバーライドする
	virtual void Event();
	virtual void Init();

	// カメラオブジェクト
	std::shared_ptr<CameraBase> m_camera = nullptr;

	// 全オブジェクトのアドレスをリストで管理
	std::list<std::shared_ptr<KdGameObject>> m_objList;
};
