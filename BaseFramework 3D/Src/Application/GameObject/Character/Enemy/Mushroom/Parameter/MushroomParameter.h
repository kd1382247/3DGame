#pragma once


class MushroomParameter
{
public:

	// 値の単位は Inspector に表示される。m=メートル s=秒 F=フレーム(60F=1秒) °=度 m/F=1フレームの移動量
	struct Parameter
	{
		//---- Status ----
		int m_maxHP = 100;   // [HP] 最大HP

		//---- Move ----
		float m_moveSpeed = 0.15f;   // [m/F] 移動速度(1フレームの移動量。実際の速さは 値 x 60 m/秒)
		float m_turnSpeed = 12.0f;   // [°/F] 回転速度(1フレームに回転できる最大角度)
		float m_reachDistance = 1.5f;   // [m] 到達判定の距離(この距離まで近づいたら攻撃を開始する)
		float m_jumpPow = 0.4f;   // ジャンプ力(飛び出しの勢い)

		//---- Attack ----
		float m_attackPower = 10.0f;   // 攻撃力
		float m_attackCooldown = 1.0f;   // [秒] 攻撃後、次の攻撃までの待ち時間
		float m_knockBackPower = 0.05f;   // ノックバックの強さ

		//---- Attack Hit ----
		float m_hitStartFrame = 24.0f;   // [F] 攻撃判定が出始めるフレーム(攻撃アニメーション開始から)
		float m_hitEndFrame = 28.0f;   // [F] 攻撃判定が消えるフレーム(攻撃アニメーション開始から)
		float m_hitRadius = 0.6f;   // [m] 攻撃判定の球の半径
		float m_hitForwardOffset = 0.8f;   // [m] 攻撃判定の球を、正面へどれだけ前に出すか
	};

	// Mushroomのタイプ(タイプごとに違う値を持たせる)
	enum class MushroomType
	{
		Smile,
		Angry
	};

	// タイプに応じたパラメータを取得
	const Parameter& GetParam(const MushroomType type)const;

	// Angryタイプになる確率(%)
	float GetAngryRate()const { return m_angryRate; }

	void Init();

	void DrawInspecter();


private:

	// 1つ分のパラメータをグループごとに描画する
	void DrawParameter(Parameter& param);

	void SaveToJson();
	void LoadFromJson();

	Parameter m_paramSmile = {};
	Parameter m_paramAngry = {};

	float m_angryRate = 30.0f;   // Angryタイプになる確率(%)
};
