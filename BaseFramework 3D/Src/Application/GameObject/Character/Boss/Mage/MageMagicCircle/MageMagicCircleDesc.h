#pragma once

// 魔法円のエフェクト再生用の設定
struct MageMagicCircleEffect
{
	std::string fileName = {};
	float       size = 1.0f;
	float       speed = 1.0f;
	float       startFrame = 0.0f;
	float       endFrame = 0.0f;
};

// 魔法円1発分の設定(生成時にMageMagicCircleへ渡す)
struct MageMagicCircleDesc
{
	// 魔法円の中心座標
	Math::Vector3 pos = {};

	// 半径(m)
	float radius = 2.0f;

	// 発動までの予備動作時間(秒)
	float telegraphTime = 0.0f;

	// 予告が終わってエフェクトが出てから、当たり判定が出るまでの時間(秒)
	float hitDelay = 0.0f;

	// エフェクトがこの進行度(0〜1)に達したら、当たり判定を終えて範囲の円も消す
	float hitEndProgress = 0.8f;

	float damage = 0.0f;

	MageMagicCircleEffect effect = {};

	// 魔法円のエフェクトが出るときに鳴らすSEのファイルパス(空なら鳴らさない)
	std::string sePath = {};
};
