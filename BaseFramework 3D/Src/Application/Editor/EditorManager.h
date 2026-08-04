#pragma once

#include"Hierarchy/EditorHierarchy.h"
#include"EditorInspector/EditorInspector.h"

class EditorManager
{
public:

	enum class EditorMode
	{
		Edit,
		Play
	};


	void Init();

	void Draw();

	void DrawMenu();



	void SetMode(EditorMode mode)  {m_mode = mode;}
	EditorMode GetMode()const      {return m_mode;}

	bool IsEditMode()const         { return m_mode == EditorMode::Edit; }

	bool IsPlayMode()const         { return m_mode == EditorMode::Play; }

	// 現在選択されているオブジェクト
	void SetSelectedObject(const std::shared_ptr<KdGameObject>&_obj)
	{
		m_spSelectedObject = _obj;
	}

	void RequestCreateObject(const std::string name);

	void SetupObjectReferences();

	std::shared_ptr<KdGameObject> CreateObject(const std::string& className);

	const std::shared_ptr<KdGameObject>&GetSelectedObject()
	{
		return m_spSelectedObject;
	}

	void SaveScene();

	void LoadScene();


private:

	std::shared_ptr<KdGameObject>m_spSelectedObject;
	
	EditorHierarchy m_hierarchy;
	EditorInspector m_inspector;

	KdGameObjectFactory m_gameObjectFactory;

	EditorMode m_mode = EditorMode::Edit;



private: // シングルトンパターン

	EditorManager() { Init(); }
	~EditorManager(){}

public:

	static EditorManager&Instance()
	{
		static EditorManager instance;
		return instance;
	}

};