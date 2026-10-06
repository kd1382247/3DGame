#pragma once

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 設定画面 (今は BGM / SE の音量調節)
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// どのシーンでも、生成して AddObject するだけで使える
// 音量の値そのものは KdAudioManager が持っているので、シーンが変わっても保持される
// Open() で開き、閉じるボタンか Close() で閉じる。開いている間だけ更新・描画される
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class Setting : public KdGameObject
{
public:

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	// 設定画面を開く / 閉じる
	void Open();
	void Close();

	// 設定画面が開いているか
	bool IsOpen()const { return m_isOpen; }

	// SEスライダーを離した時に鳴らす確認用SEのファイル名 (空なら鳴らさない)
	void SetPreviewSE(const std::string& fileName) { m_previewSEName = fileName; }

	static bool LoadVolume();
	static bool SaveVolume();

private:

	void InitTexture(std::shared_ptr<KdTexture>& tex, const std::string& fileName);

	// どのスライダーを操作しているか
	enum class SliderType
	{
		None,
		BGM,
		SE
	};

	// スライダーの値(0.0〜1.0)の取得・設定 (実際の値は KdAudioManager が持つ)
	float GetSliderValue(SliderType type)const;
	void  SetSliderValue(SliderType type, float value);

	// マウスの当たり判定
	bool IsHoverSlider(const POINT& mousePos, int barY)const;
	bool IsHoverCloseButton(const POINT& mousePos)const;

	// マウスのX座標から、スライダーの値(0.0〜1.0)を求める
	float CalcValueFromMouseX(int mouseX)const;

	// 描画
	void DrawSlider(int barY, float value);
	void DrawCloseButton();

	// 確認用SEを鳴らす
	void PlayPreviewSE();

	// 設定画面が開いているか
	bool m_isOpen = false;

	// 開いた直後のフレームか (開くためのクリックを、設定画面側で拾わないため)
	bool m_isJustOpened = false;

	// 前フレームでマウスの左ボタンが押されていたか (「押した瞬間」を取るため)
	bool m_isPrevMouseDown = false;

	// 閉じるボタンの上にマウスがあるか
	bool m_isHoverClose = false;

	// ドラッグ中のスライダー
	SliderType m_dragSlider = SliderType::None;

	// 確認用SEのファイル名
	std::string m_previewSEName = "";

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// テクスチャ (Asset/Textures/Setting/Volume/)
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	std::shared_ptr<KdTexture> m_spPanelTex          = nullptr;	// パネル(背景)
	std::shared_ptr<KdTexture> m_spHeadingTex        = nullptr;	// 「SETTING」の見出し
	std::shared_ptr<KdTexture> m_spHeadingDividerTex = nullptr;	// 見出しの下の区切り線
	std::shared_ptr<KdTexture> m_spLabelBGMTex       = nullptr;	// 「BGM」のラベル
	std::shared_ptr<KdTexture> m_spLabelSETex        = nullptr;	// 「SE」のラベル
	std::shared_ptr<KdTexture> m_spBarBackTex        = nullptr;	// スライダーのバー(背景)
	std::shared_ptr<KdTexture> m_spBarFillTex        = nullptr;	// スライダーのバー(値の分だけ表示する色つき部分)
	std::shared_ptr<KdTexture> m_spKnobTex           = nullptr;	// スライダーのつまみ
	std::shared_ptr<KdTexture> m_spCloseButtonTex    = nullptr;	// 閉じるボタン

	const std::string TexFilePath = "Asset/Textures/Setting/Volume/";
};
