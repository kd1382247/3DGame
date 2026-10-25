#pragma once

// 弾のエフェクト再生用の設定
// (初期値は、これまで使っていたEnergyBulletと同じ)
struct MageBoltEffect
{
	std::string fileName   = "Boss/Mage/Bolt/Bolt.efkefc";
	float       size       = 0.5f;
	float       speed      = 0.8f;
	float       startFrame = 20.0f;
	float       endFrame   = 30.0f;
};

// 弾1発分の設定(生成時にMageBoltへ渡す)
struct MageBoltDesc
{
	// 発射座標
	Math::Vector3 pos = {};
	// 進行方向(発射時に決定。正規化済みの値を渡す)
	Math::Vector3 dir = {};

	// [m/F] 弾の速さ
	float speed = 0.12f;
	// [m] 弾の当たり判定の半径
	float radius = 0.5f;

	float damage = 0.0f;
	float knockBackPower = 0.0f;

	// [秒] 弾が消えるまでの時間
	float lifeTime = 4.0f;

	MageBoltEffect effect = {};
};
