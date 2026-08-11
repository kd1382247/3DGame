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

	// 指定カテゴリに登録されている生成候補を表示する
	void DrawAddObjectList(KdGameObject::ObjectCategory objectCategory);

	// 指定カテゴリの配置済みオブジェクトを表示する
	void DrawObjectList(KdGameObject::ObjectCategory objectCategory);

	// オブジェクト選択
	void SelectObject(const std::shared_ptr<KdGameObject>& obj);

	HierarchyCategory m_category = HierarchyCategory::GameObject;
};
