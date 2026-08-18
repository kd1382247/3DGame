#pragma once

class Inspector
{
public:
	void Draw();

private:

	void DrawDeleteButton(const std::shared_ptr<KdGameObject> obj);

	std::unordered_map<std::string, ImVec2>m_windowSizes;

	std::string m_lastObjectType;

};