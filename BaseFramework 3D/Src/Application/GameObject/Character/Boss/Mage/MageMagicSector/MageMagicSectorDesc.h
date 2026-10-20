#pragma once

// 扇形攻撃のエフェクト再生用の設定
struct MageMagicSectorEffect
{
	std::string   fileName = {};
	float         size = 1.0f;
	float         speed = 1.0f;
	float         startFrame = 0.0f;
	float         endFrame = 0.0f;
	Math::Vector3 rotation = {};
};

// 扇形攻撃1発分の設定(生成時にMageMagicSectorへ渡す)
struct MageMagicSectorDesc
{
	// 扇形の頂点(ボスの座標)
	Math::Vector3 pos = {};

	// 扇形の正面の向き
	Math::Vector3 dir = {};

	// 扇形の角度(度)
	float angleDeg = 0.0f;

	// 扇形の半径(m)
	float radius = 0.0f;

	// 発動までの予備動作時間(秒)
	float telegraphTime = 0.0f;

	// 予告が終わってエフェクトが出てから、当たり判定が出るまでの時間(秒)
	float hitDelay = 0.0f;

	// エフェクトがこの進行度(0〜1)に達したら、当たり判定を終えて範囲の円も消す
	float hitEndProgress = 0.6f;

	float damage = 0.0f;

	MageMagicSectorEffect effect = {};
};
