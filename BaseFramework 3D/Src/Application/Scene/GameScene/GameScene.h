#pragma once

#include"../BaseScene/BaseScene.h"

#include"../../GameObject/Camera/EditorCamera/EditorCamera.h"

class WayPointManager;

class GameScene : public BaseScene
{
public :

	GameScene()  { Init(); }
	~GameScene() {}

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

	std::shared_ptr<EditorCamera>m_spEditorCamera=nullptr;

	std::shared_ptr<WayPointManager>m_spWayPointManager = nullptr;

};

