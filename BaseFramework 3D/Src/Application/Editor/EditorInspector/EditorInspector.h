#pragma once

class EditorInspector
{
public:

	void  Draw();

private:

	void DrawStageInspecter();
	void DrawParameterInspecter();


	void Delete(const std::shared_ptr<KdGameObject>& obj);

};