#pragma once

class Inspector
{
public:
	void Draw();

private:

	void DrawDeleteButton(const std::shared_ptr<KdGameObject>& obj);
};