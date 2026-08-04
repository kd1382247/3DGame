#pragma once

class EditorInspector
{
public:

	void  Draw();

private:
	
	void SaveParameter(const std::shared_ptr<KdGameObject>& obj);

	void Delete(const std::shared_ptr<KdGameObject>& obj);

};