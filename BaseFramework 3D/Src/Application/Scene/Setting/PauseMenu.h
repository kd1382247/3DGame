#pragma once

class Setting;

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// ポーズメニュー (ゲーム中にTabキーで開く「CONTROLS / SETTING / TITLE」の3択)
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// GameScene が生成して AddObject する。開閉は GameScene が Open() / Back() で行う
// 画面の流れ :
//   (ゲーム) ─Tab→ メニュー ─CONTROLS→ 操作説明
//                          ├SETTING → 設定画面 (Setting)
//                          └TITLE   → タイトルへ戻る (選ばれたことを IsTitleRequested() で GameScene が聞く)
//   操作説明 / 設定画面 ─Tab→ メニュー ─Tab→ (ゲーム)
//   設定画面の×ボタンでも、メニューへ戻る
// 操作 : マウス(乗せて選ぶ・クリックで決定) / W,S,↑,↓ で選ぶ・Enterで決定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class PauseMenu : public KdGameObject
{
public:

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	// 設定画面(SETTINGを選んだ時に開く)を教える。Init の後に呼ぶ
	void SetSetting(const std::shared_ptr<Setting>& spSetting) { m_wpSetting = spSetting; }

	// メニューを開く (ゲームから呼ぶ)
	void Open();

	// 1つ前の画面へ戻る (Tabキーで呼ぶ)
	// 操作説明・設定画面 → メニュー / メニュー → 閉じる
	void Back();

	// メニュー・操作説明・設定画面のどれかが開いているか (開いている間は、ゲームを止める)
	bool IsOpen()const { return m_page != Page::Closed; }

	// TITLE が選ばれたか (選ばれたら GameScene が、タイトルへ移動する。選ばれた後は、入力を受け付けない)
	bool IsTitleRequested()const { return m_isTitleRequested; }

private:

	// 今開いている画面
	enum class Page
	{
		Closed,		// 閉じている (ゲーム中)
		Menu,		// CONTROLS / SETTING の選択
		Controls,	// 操作説明
		Setting		// 設定画面 (表示や入力は Setting が行う)
	};

	// メニューの項目
	// (上から順に並んでいる。数字の順番が、画面の上からの並びと同じ)
	enum class Item
	{
		Controls,
		Setting,
		Title,
		None
	};

	// メニューの入力処理
	void UpdateMenu(bool isMouseTrigger, bool isMouseMoved, const POINT& mousePos);

	// 項目を選ぶ / 決定する
	void Select(Item item);
	void Decide(Item item);

	// 画面を切り替える (メニューへ戻る時は、同じフレームの入力で決定してしまわないよう、そのフレームの入力を無視する)
	void ChangePage(Page page);

	// マウスが乗っている項目 (どれにも乗っていなければ false)
	bool GetHoverItem(const POINT& mousePos, Item& outItem)const;

	// 描画
	void DrawButton(Item item, const std::shared_ptr<KdTexture>& tex);

	// 今の画面
	Page m_page = Page::Closed;

	// 今選ばれている項目
	Item m_selected = Item::Controls;

	// TITLE が選ばれたか
	bool m_isTitleRequested = false;

	// 画面を切り替えたフレームか (このフレームの入力は受け付けない)
	bool m_isIgnoreInput = false;

	// 前フレームの入力 (「押した瞬間」「マウスが動いた」を取るため。閉じている間も毎フレーム更新する)
	bool  m_isPrevMouseDown = false;
	POINT m_prevMousePos    = {};

	// 設定画面
	std::weak_ptr<Setting> m_wpSetting;

	// テクスチャ (Asset/Textures/UI/Setting/Controls/)
	std::shared_ptr<KdTexture> m_spControlsButtonTex = nullptr;	// 「CONTROLS」のボタン
	std::shared_ptr<KdTexture> m_spSettingButtonTex  = nullptr;	// 「SETTING」のボタン
	std::shared_ptr<KdTexture> m_spTitleButtonTex    = nullptr;	// 「TITLE」のボタン
	std::shared_ptr<KdTexture> m_spControlsPanelTex  = nullptr;	// 操作説明のパネル

	const std::string TexFilePath = "Asset/Textures/UI/Setting/Controls/";

};
