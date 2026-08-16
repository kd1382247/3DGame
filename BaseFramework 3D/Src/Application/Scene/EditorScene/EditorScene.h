#pragma once

#include"../BaseScene/BaseScene.h"

#include"../../GameObject/Camera/EditorCamera/EditorCamera.h"

class EditorScene : public BaseScene
{
public:

	EditorScene() { Init(); }
	~EditorScene() {}

	void EditorUpdate()override;

	void PreDraw()override;
	void DrawDebug()override;

	void ClearObjectList()override;
	void RestoreObjList()override;
	void ClearBackupList()override;

private:

	bool UseEditorMode()const override
	{
		return true;
	}

	void SetupObjectReferences();

	void Event() override;
	void Init()  override;

	std::shared_ptr<EditorCamera>m_spEditorCamera = nullptr;

	std::list<std::shared_ptr<KdGameObject>> m_spBackupList;
};

