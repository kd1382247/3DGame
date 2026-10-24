#pragma once

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 部屋に入った時の、ミッションの文字 (「ウェーブを耐え抜け！」「敵をすべて倒せ！」)
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// 文字が、ゆっくり現れる → しばらく見せる → 消える の順に動く
// 消え終わったら IsFinished() が true になる
// Room が生成して AddObject し、IsFinished() を見て、戦闘開始に進む
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class RoomIntroUI : public KdGameObject
{
public:

	// 表示するミッション
	enum class Mission
	{
		Wave,	// ウェーブを耐え抜け！
		AllKill	// 敵をすべて倒せ！
	};

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	// 表示を始める
	void Start(Mission mission);

	// 文字が消え終わったか
	bool IsFinished()const { return m_isFinished; }

private:

	// 表示が始まってからの経過時間 (秒)
	float m_timer = 0.0f;

	// 表示中か / 消え終わったか
	bool m_isPlaying  = false;
	bool m_isFinished = false;

	// 文字の画像 (Asset/Textures/UI/Stage/Intro/)
	std::shared_ptr<KdTexture> m_spTextTex = nullptr;
};
