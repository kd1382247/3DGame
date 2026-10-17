#pragma once

class EnemyCounter :public KdGameObject
{
public:

	EnemyCounter(){}
	~EnemyCounter(){}

	void Init()override;
	void Update()override;
	void DrawSprite()override;


private:

	void InitTexture(std::shared_ptr<KdTexture>& tex, const std::string& filePath);

	// 敵の数を表示するか(ボス部屋・Safeの部屋・部屋の外では表示しない)
	bool IsDrawCounter()const;

	// 今いる部屋の範囲内にいる敵の数を数える(ボスは数えない)
	int CountEnemies()const;

	// 敵アイコンを描画
	void DrawEnemyIcon();

	// 敵の数(数字)を描画
	void DrawEnemyNumber();


	std::shared_ptr<KdTexture>m_spEnemyIconTex = nullptr;
	std::shared_ptr<KdTexture>m_spNumberTex = nullptr;

	// 今いる敵の数
	int m_enemyCount = 0;

	// ===== 数字画像(Numbers.png)の情報 =====
	// 0〜9が横一列に均等に並んだ画像(全体 400 x 64)
	static constexpr int kNumberTexCellW = 40;
	static constexpr int kNumberTexCellH = 64;

	// ===== 表示する桁数 =====
	static constexpr int kMaxDigit = 3;
	static constexpr int kMaxCount = 999;

	// ===== 画面上の大きさ・位置(解像度 1280 x 720、画面中心が原点) =====
	// 数字1桁の表示サイズ(元画像と同じ縦横比 40:64)
	static constexpr int kDigitW = 40;
	static constexpr int kDigitH = 64;

	// アイコンの表示サイズ
	static constexpr int kIconW = 100;
	static constexpr int kIconH = 60;

	// アイコンと数字の間の隙間
	static constexpr int kIconNumberGap = 10;

	// 画面右端・上端からの余白
	static constexpr int kMarginRight = 30;
	static constexpr int kMarginTop   = 30;

	// 画面の右端・上端の座標
	static constexpr int kScreenRight = 640;
	static constexpr int kScreenTop   = 360;

	// Texture描画の基準点(左端・縦の中央)
	const Math::Vector2 m_pivot = { 0.0f,0.5f };

};
