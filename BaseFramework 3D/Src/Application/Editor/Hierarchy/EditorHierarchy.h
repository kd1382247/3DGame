#pragma once

class EditorHierarchy
{
public:


	enum class EditorObjectType
	{
		GameObject,
		WayPoint
	};

	void Draw();

private:

	void DrawGameObjectHierarchy();
	void DrawWayPointHierarchy();


	EditorObjectType m_editorObjectType = EditorObjectType::GameObject;
};