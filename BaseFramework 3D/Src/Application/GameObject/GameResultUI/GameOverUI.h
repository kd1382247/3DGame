#pragma once

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// ゲームオーバーの画面 (暗転 → 「GAME OVER」 → 「リトライ / タイトルへ」の2択)
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// GameScene が生成して AddObject する。選ばれた項目は GetChoice() で聞く
// 操作 : マウス(乗せて選ぶ・クリックで決定) / A,D,←,→ で選ぶ・Enterで決定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class GameOverUI : public KdGameObject
{
public:

	// 選べる項目
	enum class Choice
	{
		None,
		Retry,	// ステージの最初からやり直す
		Title	// タイトルへ戻る
	};

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	// ボタンを選べる状態か (GameScene が、カーソルを出す合図に使う)
	bool IsMenuOpen()const { return m_isMenuOpen; }

	// 決定された項目 (まだ決定されていなければ None)
	Choice GetChoice()const { return m_choice; }

private:

	// ボタンを選べる状態にする
	void OpenMenu();

	// ボタンの操作 (キー・マウス)
	void UpdateMenu();

	// 項目を選ぶ / 決定する
	void Select(Choice choice);
	void Decide(Choice choice);

	// マウスが乗っているボタン (どれにも乗っていなければ None)
	Choice GetHoverButton(const POINT& mousePos)const;

	// 画像を描く (x,y は画像の中心。scale は大きさの倍率)
	void DrawImage(std::shared_ptr<KdTexture> tex, int x, int y, float scale, float alpha);

	// ボタンを描く
	void DrawButton(Choice choice, std::shared_ptr<KdTexture> tex, float alpha);

	// 演出が始まってからの経過時間 (秒)
	float m_timer = 0.0f;

	// ボタンを選べる状態か
	bool m_isMenuOpen = false;

	// 今選ばれている項目 / 決定された項目
	Choice m_selected = Choice::Retry;
	Choice m_choice   = Choice::None;

	// 前フレームの入力 (「押した瞬間」「マウスが動いた」を取るため)
	bool  m_isPrevMouseDown = false;
	POINT m_prevMousePos    = {};

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// テクスチャ (Asset/Textures/UI/Result/)
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	std::shared_ptr<KdTexture> m_spTextTex        = nullptr;	// 「GAME OVER」の文字
	std::shared_ptr<KdTexture> m_spRetryButtonTex = nullptr;	// 「RETRY」のボタン
	std::shared_ptr<KdTexture> m_spTitleButtonTex = nullptr;	// 「TITLE」のボタン
};
