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

	/*enum class EditorCategory
	{
		Stage,
		Parameter
	};*/


	void Init();

	void Draw();

	void DrawMenu();



	void SetEditorMode(EditorMode state)  {m_editorMode = state;}
	EditorMode GetEditorMode()const      {return m_editorMode;}

	// エディタの状態
	bool IsEditMode()const         { return m_editorMode == EditorMode::Edit; }
	bool IsPlayMode()const         { return m_editorMode == EditorMode::Play; }


	//void SetEditorCategory(EditorCategory category) { m_editorCategory = category; }
	//EditorCategory GetEditorCategory() { return m_editorCategory; }

	//// エディタの状態
	//bool IsStageEdit()const { return m_editorCategory==EditorCategory::Stage; }
	//bool IsParameterEdit()const { return m_editorCategory == EditorCategory::Parameter; }

	// 現在選択されているオブジェクト
	void SetSelectedObject(const std::shared_ptr<KdGameObject>&_obj)
	{
		m_spSelectedObject = _obj;
	}


	void SetupObjectReferences();

	void  CreateGameObject(const std::string& className);

	const std::shared_ptr<KdGameObject>&GetSelectedObject()
	{
		return m_spSelectedObject;
	}

	void SaveScene();

	void LoadScene();


private:

	std::shared_ptr<KdGameObject>m_spSelectedObject;
	
	EditorHierarchy     m_hierarchy;
	EditorInspector     m_inspector;

	
	EditorMode          m_editorMode = EditorMode::Edit;
	//EditorCategory      m_editorCategory=EditorCategory::Stage;


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