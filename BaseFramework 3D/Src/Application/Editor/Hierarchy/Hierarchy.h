#pragma once

class Hierarchy
{
public:

	enum class HierarchyCategory
	{
		GameObject,
		WayPoint,
		Stage,
		CollisionBox,
		OBB
	};


	void Draw();

	HierarchyCategory GetHierarchyCategory() const { return m_category; }

private:

	// カテゴリ選択(ボタンを押すと一覧が出て選択でき、選んだカテゴリ名をボタンの隣に表示する)
	void DrawCategorySelector();
	void CategorySelectItem(const char* label, HierarchyCategory category);
	static const char* GetCategoryLabel(HierarchyCategory category);

	// 選択中カテゴリのAddボタン
	void DrawAddButtons();

	void AddGameObject();
	void AddWayPoint();
	void AddStage();
	void AddCollisionBox();
	void AddOBB();

	// 検索ボックス(名前の一部で一覧を絞り込む)
	void DrawSearchFilter();
	bool MatchesSearchFilter(const std::string& name) const;

	// 選択中カテゴリのオブジェクト一覧
	void DrawSelectedCategoryList();

	void DrawGameObjects();
	void DrawWayPoints();
	void DrawStage();
	void DrawCollisionBox();
	void DrawOBB();


	// 指定のカテゴリに登録されている生成候補を表示する
	void DrawAddObjectList(KdGameObject::ObjectCategory objectCategory);

	// 指定されたカテゴリを表示
	void DrawObjectList(KdGameObject::ObjectCategory objectCategory);


	// オブジェクト選択
	void SelectHierarchyObject(const std::shared_ptr<KdGameObject>& obj);

	// 現在選択中のカテゴリ(シーンビューでのクリック選択対象にもなる)
	HierarchyCategory m_category = HierarchyCategory::GameObject;

	// 検索ボックスの入力内容
	char m_searchFilter[128] = "";
};
