#pragma once

// ビームのエフェクト再生用の設定
struct MageBeamEffect
{
	std::string   fileName = {};
	float         size = 1.0f;
	float         speed = 1.0f;
	float         startFrame = 0.0f;
	float         endFrame = 0.0f;
	Math::Vector3 rotation = {};
	Math::Vector3 effectPos = {};
};

// ビーム1発分の設定(生成時にMageBeamへ渡す)
struct MageBeamDesc
{
	Math::Vector3 pos = {};
	Math::Vector3 dir = {};

	float length = 0.0f;
	float width = 0.0f;

	float damage = 0.0f;

	// ビームが出続ける時間(秒)
	float duration = 0.0f;

	// ビームが出てから、最初の当たり判定が出るまでの時間(秒。durationの中に含まれる)
	float hitDelay = 0.0f;

	// 当たり続けている間の、再ヒットまでの間隔(秒)
	float hitInterval = 0.4f;

	MageBeamEffect effect = {};
};
