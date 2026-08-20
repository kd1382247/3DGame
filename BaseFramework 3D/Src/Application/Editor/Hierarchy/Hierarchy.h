#pragma once

class Hierarchy
{
public:
	
	enum class HierarchyCategory
	{
		GameObject,
		WayPoint,
		Stage,
		CollisionBox
	};


	void Draw();

	HierarchyCategory GetHierarchyCategory(){return m_category; }

private:


	void DrawCategoryButtons();
	// 描画するカテゴリを選択するボタン
	void CategoryButton(const char*label,HierarchyCategory category);

	// オブジェクトを追加するボタン
	void DrawAddButtons();

	void AddGameObject();
	void AddWayPoint();
	void AddStage();
	void AddCollisionBox();

	
	void DrawScrollableList();

	void DrawGameObjects();
	void DrawWayPoints();
	void DrawStage();
	void DrawCollisionBox();

	// 指定のカテゴリに登録されている生成候補を表示する
	void DrawAddObjectList(KdGameObject::ObjectCategory objectCategory);

	// 指定されたカテゴリを表示
	void DrawObjectList(KdGameObject::ObjectCategory objectCategory);


	// オブジェクト選択
	void SelectObject(const std::shared_ptr<KdGameObject>& obj);

	HierarchyCategory m_category = HierarchyCategory::GameObject;
};
