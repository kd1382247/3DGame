#pragma once

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// ゲームクリアの画面 (少し暗くして「STAGE CLEAR」を出す)
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// GameScene が生成して AddObject する。演出が終わったかは IsFinished() で聞く
// 操作はない (終わったら GameScene がリザルトへ進める)
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class GameClearUI : public KdGameObject
{
public:

	void Init()override;
	void Update()override;
	void DrawSprite()override;

	// 演出が終わったか
	bool IsFinished()const { return m_timer >= FinishTime; }

private:

	// 演出が終わるまでの時間 (秒)
	static constexpr float FinishTime = 4.0f;

	// 演出が始まってからの経過時間 (秒)
	float m_timer = 0.0f;

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// テクスチャ (Asset/Textures/UI/Result/)
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	std::shared_ptr<KdTexture> m_spTextTex = nullptr;	// 「STAGE CLEAR」の文字
};
