#pragma once

class Hierarchy
{
public:


	enum class HierarchyCategory
	{
		GameObject,
		WayPoint,
		Stage
	};

	void Draw();

private:

	void DrawGameObjects();
	void DrawWayPoints();
	void DrawStage();

	// オブジェクト追加
	void AddObject(const auto& createFanctions,const KdGameObject::ObjectCategory objectCategory);

	// オブジェクト選択
	void SelectObject(const std::shared_ptr<KdGameObject>& obj);


	HierarchyCategory m_category = HierarchyCategory::GameObject;
};