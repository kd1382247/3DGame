#pragma once

#include"../BaseScene/BaseScene.h"

#include"../../GameObject/Camera/EditorCamera/EditorCamera.h"

class WayPointManager;

class EditorScene : public BaseScene
{
public:

	EditorScene() { Init(); }
	~EditorScene() {}

	void EditorUpdate()override;

	void PreDraw()override;
	void DrawDebug()override;
private:

	bool UseEditorMode()const override
	{
		return true;
	}

	void SetupObjectReferences();

	void Event() override;
	void Init()  override;

	std::shared_ptr<EditorCamera>m_spEditorCamera = nullptr;

	std::shared_ptr<WayPointManager>m_spWayPointManager = nullptr;

};

