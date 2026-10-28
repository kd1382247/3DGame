#pragma once

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// リザルトの画面 (「Result」の文字 + クリアタイム + 「リトライ / タイトルへ」の2択)
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// ResultScene が生成して AddObject する。選ばれた項目は GetChoice() で聞く
// クリアタイムは GameResult から読む (ゲームシーンが、ボスを倒した時に記録する)
// 操作 : マウス(乗せて選ぶ・クリックで決定) / A,D,←,→ で選ぶ・Enterで決定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class ResultUI : public KdGameObject
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

	// クリアタイムを「分:秒」の数字の画像で描く (画面の中央に、横に並べる)
	void DrawClearTime(int y, float alpha);

	// 数字を1つ描く (x,y は画像の中心)
	void DrawDigit(int digit, int x, int y, float alpha);

	// 表示が始まってからの経過時間 (秒)
	float m_timer = 0.0f;

	// ボタンを選べる状態か
	bool m_isMenuOpen = false;

	// 今選ばれている項目 / 決定された項目
	Choice m_selected = Choice::Retry;
	Choice m_choice   = Choice::None;

	// 表示するクリアタイム (秒)
	float m_clearTime = 0.0f;

	// 前フレームの入力 (「押した瞬間」「マウスが動いた」を取るため)
	bool  m_isPrevMouseDown = false;
	POINT m_prevMousePos    = {};

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// テクスチャ (Asset/Textures/UI/Result/)
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	std::shared_ptr<KdTexture> m_spBackTex        = nullptr;	// 背景
	std::shared_ptr<KdTexture> m_spResultTextTex  = nullptr;	// 「Result」の文字
	std::shared_ptr<KdTexture> m_spFrameTex       = nullptr;	// クリアタイムの枠
	std::shared_ptr<KdTexture> m_spTimeTextTex    = nullptr;	// 「ClearTime」の文字
	std::shared_ptr<KdTexture> m_spDigitsTex      = nullptr;	// 数字 (0～9が横に並んだ1枚の画像)
	std::shared_ptr<KdTexture> m_spColonTex       = nullptr;	// 「:」
	std::shared_ptr<KdTexture> m_spRetryButtonTex = nullptr;	// 「RETRY」のボタン
	std::shared_ptr<KdTexture> m_spTitleButtonTex = nullptr;	// 「TITLE」のボタン
};
