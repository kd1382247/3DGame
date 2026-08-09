#pragma once

class EditorInspector
{
public:

	void  Draw();

private:

	void DrawGameObjectInspecter();
	void DrawWayPointInspecter();
	


	void Delete(const std::shared_ptr<KdGameObject>& obj);

};