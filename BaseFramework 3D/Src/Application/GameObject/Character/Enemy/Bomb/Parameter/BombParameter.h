#pragma once


class BombParameter
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
		float m_attackPow = 10.0f;   // 爆発のダメージ
		float m_attackCooldown = 1.0f;   // [秒] 攻撃後、次の攻撃までの待ち時間

		//---- Explosion ----
		float m_explosionRadius = 3.0f;   // [m] 爆発の半径
		float m_chargeDuration = 1.5f;   // [秒] 爆発前のため(予告)時間
		float m_explosionKnockBack = 0.5f;   // 爆発が当たった時のノックバックの強さ
	};

	const Parameter& GetParam()const { return m_param; }

	void Init();

	void DrawInspecter();


private:

	void SaveToJson();
	void LoadFromJson();

	Parameter m_param = {};
};
