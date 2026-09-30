#pragma once


class StarFishParameter
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
		float m_reachDistance = 5.0f;   // [m] 到達判定の距離(この距離まで近づいたら攻撃を開始する)
		float m_jumpPow = 0.4f;   // ジャンプ力(飛び出しの勢い)

		//---- Attack ----
		float m_attackPow = 10.0f;   // Energy弾のダメージ
		float m_attackCooldown = 1.0f;   // [秒] 攻撃後、次の攻撃までの待ち時間

		//---- Bullet ----
		float m_bulletFireFrame = 16.0f;   // [F] Energy弾を発射するフレーム(攻撃アニメーション開始から)
		float m_bulletSpeed = 0.08f;   // [m/F] Energy弾の速さ
		float m_bulletRadius = 0.05f;   // [m] Energy弾の半径
		float m_bulletKnockBack = 0.08f;   // Energy弾が当たった時のノックバックの強さ
		float m_bulletLifeTime = 4.0f;   // [秒] Energy弾が消えるまでの時間
		float m_bulletSpawnHeight = 0.8f;   // [m] 発射位置の高さ(口元)
		float m_bulletSpawnForward = 0.3f;   // [m] 発射位置を、正面へどれだけ前に出すか
		float m_bulletAimHeight = 0.8f;   // [m] 狙う位置の高さ(プレイヤーの足元からの高さ)
	};

	const Parameter& GetParam()const { return m_param; }

	void Init();

	void DrawInspecter();


private:

	void SaveToJson();
	void LoadFromJson();

	Parameter m_param = {};
};
