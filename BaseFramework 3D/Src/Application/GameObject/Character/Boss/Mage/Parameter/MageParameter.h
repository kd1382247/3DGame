#pragma once


class MageParameter
{
public:

	// 値の単位は Inspector に表示される。m=メートル s=秒 F=フレーム(60F=1秒) °=度 m/F=1フレームの移動量
	struct Parameter
	{
		//---- Status ----
		int m_maxHP = 100;   // [HP] 最大HP
		float m_scale = 1.5f;   // [倍] 大きさ(倍率)

		//---- Move ----
		float m_moveSpeed = 0.15f;   // [m/F] 移動速度(1フレームの移動量。実際の速さは 値 x 60 m/秒)
		float m_turnSpeed = 12.0f;   // [°/F] 回転速度(1フレームに回転できる最大角度)
		float m_reachDistance = 5.0f;   // [m] 到達判定の距離(この距離まで近づいたら攻撃を開始する)
		float m_reachDistanceMargin = 6.0f;   // [m] 到達後、この距離より離れたら再び追いかける
		float m_jumpPow = 0.4f;   // ジャンプ力(飛び出しの勢い)

		//---- Attack ----
		float m_attackPow = 10.0f;   // 魔法円・魔法弾・全方位魔法・ビームのダメージ
		float m_attackCooldown = 2.0f;   // [秒] 攻撃後、次の攻撃までの待ち時間

		//---- TargetCircle ----
		float m_targetCircleCastDelay = 0.4f;   // [秒] 詠唱開始から魔法円が出るまでの時間
		float m_targetCircleRadius = 2.5f;   // [m] 魔法円の半径
		float m_targetCircleTelegraph = 0.8f;   // [秒] 魔法円の予告から発動までの時間
		int m_targetCircleShotCount = 5;   // [個/回] 連続で撃つ回数

		//---- ForwardSector ----
		float m_forwardSectorCastDelay = 0.35f;   // [秒] 詠唱開始から扇形の魔法が出るまでの時間
		float m_forwardSectorAngle = 100.0f;   // [°] 扇形の角度
		float m_forwardSectorRadius = 6.5f;   // [m] 扇形の半径
		float m_forwardSectorTelegraph = 0.8f;   // [秒] 予告から発動までの時間
		float m_forwardSectorDamage = 10.0f;   // 扇形の魔法のダメージ

		//---- Bolt ----
		float m_boltCastDelay = 0.4f;   // [秒] 詠唱開始から魔法弾を撃つまでの時間
		int m_boltShotCount = 3;   // [個/回] 連続で撃つ回数
		float m_boltSpeed = 0.12f;   // [m/F] 魔法弾の速さ
		float m_boltRadius = 0.2f;   // [m] 魔法弾の半径
		float m_boltKnockBack = 0.15f;   // 魔法弾が当たった時のノックバックの強さ
		float m_boltLifeTime = 4.0f;   // [秒] 魔法弾が消えるまでの時間
		float m_boltSpawnHeight = 0.8f;   // [m] 魔法弾を出す高さ
		float m_boltAimHeight = 0.8f;   // [m] 狙う位置の高さ(プレイヤーの足元からの高さ)

		//---- NovaCircle ----
		float m_novaCircleCastDelay = 0.6f;   // [秒] 詠唱開始から全方位魔法が出るまでの時間
		float m_novaCircleRadius = 6.0f;   // [m] 全方位魔法の半径
		float m_novaCircleTelegraph = 1.2f;   // [秒] 予告から発動までの時間

		//---- Beam ----
		float m_beamCastDelay = 0.6f;   // [秒] 詠唱開始からビームが出るまでの時間
		float m_beamLength = 19.0f;   // [m] ビームの長さ
		float m_beamWidth = 2.5f;   // [m] ビームの幅
		float m_beamForwardOffset = 7.0f;   // [m] ビームの基準位置を、正面へどれだけ前に出すか
		float m_beamDuration = 3.0f;   // [秒] ビームが出続ける時間

		//---- Summon ----
		float m_summonCastDelay = 0.4f;   // [秒] 詠唱開始から煙の演出が始まるまでの時間
		float m_summonDelay = 0.6f;   // [秒] 煙の演出が始まってから敵が出るまでの時間
		int m_summonCount = 5;   // [個/回] 一度に呼び出す敵の数
		float m_summonRadius = 4.0f;   // [m] ボスを囲んで敵を出す円の半径
	};

	const Parameter& GetParam()const { return m_param; }

	void Init();

	void DrawInspecter();


private:

	void SaveToJson();
	void LoadFromJson();

	Parameter m_param = {};
};
