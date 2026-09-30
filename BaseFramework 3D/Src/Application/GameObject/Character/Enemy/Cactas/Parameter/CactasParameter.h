#pragma once


class CactasParameter
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
		float m_attackCooldown = 0.5f;   // [秒] 攻撃後、次の攻撃までの待ち時間
		float m_knockBackPower = 0.08f;   // ノックバックの強さ

		//---- Attack Hit ----
		float m_hitStartFrame = 16.0f;   // [F] 攻撃判定が出始めるフレーム(攻撃アニメーション開始から)
		float m_hitEndFrame = 20.0f;   // [F] 攻撃判定が消えるフレーム(攻撃アニメーション開始から)
		float m_hitRadius = 0.6f;   // [m] 攻撃判定の球の半径
		float m_hitForwardOffset = 0.8f;   // [m] 攻撃判定の球を、正面へどれだけ前に出すか
	};

	const Parameter& GetParam()const { return m_param; }

	void Init();

	void DrawInspecter();


private:

	void SaveToJson();
	void LoadFromJson();

	Parameter m_param = {};
};
