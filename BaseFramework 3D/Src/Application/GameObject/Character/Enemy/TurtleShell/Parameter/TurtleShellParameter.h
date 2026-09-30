#pragma once


class TurtleShellParameter
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
		float m_attackPow = 10.0f;   // 回転攻撃のダメージ
		float m_attackCooldown = 1.0f;   // [秒] 攻撃後、次の攻撃までの待ち時間
		float m_knockBackPower = 0.3f;   // 回転攻撃が当たった時のノックバックの強さ

		//---- Spin Attack ----
		float m_spinDuration = 5.0f;   // [秒] 回転攻撃を続ける時間
		float m_spinSpeedBonus = 0.07f;   // [m/F] 回転中は MoveSpeed にこの値を足した速さで動く
		float m_spinHitRadius = 0.5f;   // [m] 回転攻撃の判定の球の半径
		float m_spinHitCooldown = 0.5f;   // [秒] 一度当たってから、次に当たるようになるまでの時間

		//---- Dizzy ----
		float m_dizzyDuration = 3.0f;   // [秒] 回転攻撃のあと、目を回して止まっている時間
	};

	const Parameter& GetParam()const { return m_param; }

	void Init();

	void DrawInspecter();


private:

	void SaveToJson();
	void LoadFromJson();

	Parameter m_param = {};
};
