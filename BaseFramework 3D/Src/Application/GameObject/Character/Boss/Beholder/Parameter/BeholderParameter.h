#pragma once


class BeholderParameter
{
public:


	//==================================================================
	// エフェクト1つ分の設定(攻撃ごとに持つ)
	// ファイル名はコード側で固定する(Inspector / json の対象外)
	//==================================================================
	struct EffectParam
	{
		float m_size       = 0.5f;     // [倍] エフェクトの大きさ
		float m_speed      = 0.8f;    // [倍] 再生速度
		int   m_startFrame = 10;   // [F] 再生を始めるフレーム
		int   m_endFrame   = 50;     // [F] 再生を終えるフレーム
	};

	//==================================================================
	// 攻撃ごとのパラメータ
	// 弾の軌道が膨らむ向き(curveAngle)は、パラメータにせずコード側で固定する
	//==================================================================

	// 追従弾
	struct SparkBullet
	{
		float m_vanishT              = 1.5f;   // [進行度] 弾が消える進行度(1.0で目標地点に着いた時点。1より大きいと、通り過ぎてから消える)
		float m_curveRate            = 0.4f;   // [倍] 軌道の膨らみの大きさ(発射位置から目標までの距離に対する割合)
		float m_trackingStopDistance = 2.0f;   // [m] 追尾をやめる距離(※今は追尾の処理を止めているため、まだ使われていない)
		float m_speed                = 0.1f;   // [m/F] 弾の速さ
		float m_radius               = 0.5f;   // [m] 弾の当たり判定の半径
		float m_reflectSpeed         = 0.15f;   // [m/F] パリィで打ち返された後の速さ(※打ち返しの処理が未実装のため、まだ使われていない)
		float m_damage               = 10.0f;   // 弾のダメージ
		float m_knockBack            = 0.2f;   // 弾が当たった時のノックバックの強さ

		EffectParam m_effect;
	};

	// 溜めショット
	struct ChargeSparkBullet
	{
		float m_chargeTime   = 2.0f; // チャージ時間

		float m_radius       = 0.8;  // 当たり判定の半径
		float m_targetSize = 1.0f; // 目標の大きさ

		float m_speed        = 0.15f;// 弾の速度
		float m_reflectSpeed = 0.20f;// 反射後の弾の速度

		float m_lifeTime     = 5.0f; // 弾の寿命

		float m_damage       = 30.0f;// 弾のダメージ
		float m_knockBack    = 0.2f; // 弾が当たった時のノックバックの強さ

		EffectParam m_effect;

	};


	// 値の単位は Inspector に表示される。m=メートル s=秒 F=フレーム(60F=1秒) °=度 m/F=1フレームの移動量
	struct Parameter
	{
		//---- Status ----
		int m_maxHP = 100;   // [HP] 最大HP

		//---- Move ----
		float m_moveSpeed = 0.15f;   // [m/F] 移動速度(1フレームの移動量。実際の速さは 値 x 60 m/秒)
		float m_turnSpeed = 12.0f;   // [°/F] 回転速度(1フレームに回転できる最大角度)
		float m_reachDistance = 5.0f;   // [m] 到達判定の距離(この距離まで近づいたら攻撃を開始する)
		float m_reachDistanceMargin = 6.0f;   // [m] 到達後、この距離より離れたら再び追いかける
		float m_gravityAcceleration = 55.0f;   // [m/s^2] 重力加速度

		//---- Attack(全攻撃で共通) ----
		float m_attackCooldown = 2.0f;   // [秒] 攻撃後、次の攻撃までの待ち時間

		//---- 攻撃ごと ----
		SparkBullet m_sparkBullet;

		ChargeSparkBullet m_chargeSparkBullet;

		//---- Collision ----
		float m_bumpSphereRadius = 0.5f;   // [m] 押し戻し判定の球の半径
		float m_bumpSphereOffsetY = 1.5f;   // [m] 足元から、押し戻し判定の球の中心までの高さ(地面に立たせるなら半径と同じ値)
	};

	const Parameter& GetParam()const { return m_param; }

	void Init();

	void DrawInspecter();


private:

	void SaveToJson();
	void LoadFromJson();

	Parameter m_param = {};
};
