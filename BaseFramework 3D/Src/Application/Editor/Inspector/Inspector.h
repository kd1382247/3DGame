#pragma once

class Inspector
{
public:
	void Draw();

private:

	void DrawCopyButton(const std::shared_ptr<KdGameObject>& obj);

	void DrawDeleteButton(const std::shared_ptr<KdGameObject>& obj);

};