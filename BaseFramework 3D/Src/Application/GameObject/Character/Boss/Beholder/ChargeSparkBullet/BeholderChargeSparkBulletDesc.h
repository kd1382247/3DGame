#pragma once

// 弾のエフェクト再生用の設定
// (初期値は、これまで使っていたEnergyBulletと同じ)
struct BeholderChargeSparkBulletEffect
{
	std::string fileName = "Boss/Beholder/Spark/Spark.efkefc";
	float       size = 0.8f;
	float       speed = 0.8f;
	float       startFrame = 10.0f;
	float       endFrame = 50.0f;
};

// 弾1発分の設定(生成時にMageBoltへ渡す)
struct BeholderChargeSparkBulletDesc
{
	// 生成位置
	Math::Vector3 pos = {};

	// 弾の速度
	float speed = 0.0f;

	// [m] 弾の当たり判定の半径
	float radius = 0.0f;

	float reflectSpeed = 0.0f;

	float damage = 0.0f;
	float knockBackPower = 0.0f;

	float lifeTime = 0.0f;

	BeholderChargeSparkBulletEffect effect = {};
};
