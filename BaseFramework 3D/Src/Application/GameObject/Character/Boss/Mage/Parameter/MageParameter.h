#pragma once


class MageParameter
{
public:

	// 値の単位は Inspector に表示される。m=メートル s=秒 F=フレーム(60F=1秒) °=度 m/F=1フレームの移動量

	//==================================================================
	// エフェクト1つ分の設定(攻撃ごとに持つ)
	// ファイル名はコード側で固定する(Inspector / json の対象外)
	//==================================================================
	struct EffectParam
	{
		EffectParam() = default;

		EffectParam(const char* fileName, float size, float speed, int startFrame, int endFrame,
			float rotationY = 0.0f, float forwardOffset = 0.0f, float heightOffset = 0.0f)
			: m_fileName(fileName), m_size(size), m_speed(speed), m_startFrame(startFrame), m_endFrame(endFrame)
			, m_rotationY(rotationY), m_forwardOffset(forwardOffset), m_heightOffset(heightOffset)
		{
		}

		std::string m_fileName = {};

		float m_size = 1.0f;            // [倍] エフェクトの大きさ
		float m_speed = 1.0f;           // [倍] 再生速度
		int   m_startFrame = 0;         // [F] 再生を始めるフレーム
		int   m_endFrame = 0;           // [F] 再生を終えるフレーム
		float m_rotationY = 0.0f;       // [°] ボスの向きに足す回転(エフェクト側の正面とのズレの補正)
		float m_forwardOffset = 0.0f;   // [m] 基準位置から、ボスの前方へずらす距離
		float m_heightOffset = 0.0f;    // [m] 基準位置から、上へずらす距離
	};

	//==================================================================
	// 攻撃ごとのパラメータ
	//==================================================================

	// 2. プレイヤー位置に魔法攻撃(魔法円)
	struct TargetCircle
	{
		float m_castDelay = 0.4f;   // [秒] 詠唱開始から魔法円が出るまでの時間
		float m_radius = 2.5f;      // [m] 魔法円の半径
		float m_telegraph = 0.8f;   // [秒] 魔法円の予告から発動までの時間
		int   m_shotCount = 5;      // [個/回] 連続で撃つ回数
		float m_shotInterval = 0.8f;   // [秒] 1発の詠唱開始から、次の弾の詠唱開始までの時間(m_castDelay より短くしても、弾が出るのを待ってから次へ進む)
		float m_damage = 10.0f;     // 魔法円のダメージ
		float m_hitDelay = 0.15f;   // [秒] 予告が終わってエフェクトが出てから、当たり判定が出るまでの時間
		float m_hitEndProgress = 0.8f;   // [0〜1] エフェクトがこの進行度に達したら、当たり判定を終えて範囲の円も消す

		EffectParam m_effect{ "Tornado/Tornado.efkefc", 0.4f, 1.0f, 0, 90 };
	};

	// 3. 前方に扇形の範囲攻撃
	struct ForwardSector
	{
		float m_castDelay = 0.35f;   // [秒] 詠唱開始から扇形の魔法が出るまでの時間
		float m_angle = 100.0f;      // [°] 扇形の角度
		float m_radius = 6.5f;       // [m] 扇形の半径
		float m_telegraph = 0.8f;    // [秒] 予告から発動までの時間
		float m_damage = 10.0f;      // 扇形の魔法のダメージ
		float m_hitDelay = 0.15f;   // [秒] 予告が終わってエフェクトが出てから、当たり判定が出るまでの時間
		float m_hitEndProgress = 0.6f;   // [0〜1] エフェクトがこの進行度に達したら、当たり判定を終えて範囲の円も消す

		EffectParam m_effect{ "Sword/Sword2.efkefc", 2.5f, 1.2f, 0, 80, -60.0f };
	};

	// 4. プレイヤーへ単発魔法弾
	struct Bolt
	{
		float m_castDelay = 0.4f;     // [秒] 詠唱開始から魔法弾を撃つまでの時間
		int   m_shotCount = 3;        // [個/回] 連続で撃つ回数
		float m_speed = 0.12f;        // [m/F] 魔法弾の速さ
		float m_radius = 0.2f;        // [m] 魔法弾の半径
		float m_damage = 10.0f;       // 魔法弾のダメージ
		float m_knockBack = 0.15f;    // 魔法弾が当たった時のノックバックの強さ
		float m_lifeTime = 4.0f;      // [秒] 魔法弾が消えるまでの時間
		float m_spawnHeight = 0.8f;   // [m] 魔法弾を出す高さ
		float m_aimHeight = 0.8f;     // [m] 狙う位置の高さ(プレイヤーの足元からの高さ)
	};

	// 5. ボス中心から半径数メートルの範囲攻撃(HP50%以下限定)
	struct NovaCircle
	{
		float m_castDelay = 0.6f;   // [秒] 詠唱開始から全方位魔法が出るまでの時間
		float m_radius = 6.0f;      // [m] 全方位魔法の半径
		float m_telegraph = 1.2f;   // [秒] 予告から発動までの時間
		float m_damage = 10.0f;     // 全方位魔法のダメージ
		float m_hitDelay = 0.15f;   // [秒] 予告が終わってエフェクトが出てから、当たり判定が出るまでの時間
		float m_hitEndProgress = 0.8f;   // [0〜1] エフェクトがこの進行度に達したら、当たり判定を終えて範囲の円も消す

		EffectParam m_effect{ "Salamander/Salamander.efkefc", 0.8f, 1.0f, 0, 90 };
	};

	// 6. レーザー(ビーム)
	struct Beam
	{
		float m_castDelay = 0.6f;       // [秒] 詠唱開始からビームが出るまでの時間
		float m_length = 19.0f;         // [m] ビームの長さ
		float m_width = 2.5f;           // [m] ビームの幅
		float m_forwardOffset = 7.0f;   // [m] ビームの基準位置を、正面へどれだけ前に出すか
		float m_duration = 3.0f;        // [秒] ビームが出続ける時間(エフェクトが先に終わったら、そこで終わる)
		float m_damage = 10.0f;         // ビームのダメージ
		float m_hitDelay = 0.15f;       // [秒] ビームが出てから、最初の当たり判定が出るまでの時間(m_duration の中に含まれる)
		float m_hitInterval = 0.4f;     // [秒] ビームが当たり続けている間の、再ヒットまでの間隔

		// 詠唱中に再生する予告(windup)エフェクト
		EffectParam m_windupEffect{ "Beam/Beam.efkefc", 1.0f, 1.0f, 0, 120, 180.0f, 1.8f, 1.5f };
		// ビーム本体のエフェクト
		EffectParam m_beamEffect{ "Beam/Beam.efkefc", 0.6f, 1.0f, 130, 250, 180.0f, 1.8f, 1.5f };
	};

	// 1. 敵を召喚
	struct Summon
	{
		float m_castDelay = 0.4f;   // [秒] 詠唱開始から煙の演出が始まるまでの時間
		float m_delay = 0.6f;       // [秒] 煙の演出が始まってから敵が出るまでの時間
		int   m_count = 5;          // [個/回] 一度に呼び出す敵の数
		float m_radius = 4.0f;      // [m] ボスを囲んで敵を出す円の半径
	};

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

		//---- Attack(全攻撃で共通) ----
		float m_attackCooldown = 2.0f;   // [秒] 攻撃後、次の攻撃までの待ち時間

		//---- 攻撃ごと ----
		TargetCircle  m_targetCircle;
		ForwardSector m_forwardSector;
		Bolt          m_bolt;
		NovaCircle    m_novaCircle;
		Beam          m_beam;
		Summon        m_summon;
	};

	const Parameter& GetParam()const { return m_param; }

	void Init();

	void DrawInspecter();


private:

	void SaveToJson();
	void LoadFromJson();

	Parameter m_param = {};
};
