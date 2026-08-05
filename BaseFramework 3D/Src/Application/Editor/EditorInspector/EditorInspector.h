#pragma once

class EditorInspector
{
public:

	void  Draw();

private:

	void Delete(const std::shared_ptr<KdGameObject>& obj);

};