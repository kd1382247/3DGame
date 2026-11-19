#pragma once

// 弾のエフェクト再生用の設定
// (初期値は、これまで使っていたEnergyBulletと同じ)
struct BeholderSparkBulletEffect
{
	std::string fileName = "Boss/Beholder/Spark/Spark.efkefc";
	float       size = 0.5f;
	float       speed = 0.8f;
	float       startFrame = 10.0f;
	float       endFrame = 50.0f;
};

// 弾1発分の設定(生成時にMageBoltへ渡す)
struct BeholderSparkBulletDesc
{
	// 生成位置
	Math::Vector3 pos = {};

	// 生成するNullPointNodeの番号
	int   nullPointNumber = -1;

	// tがどれだけ進んだら弾を消すのか
	float vanishT = 1.0f;

	// 膨らむ向き
	float curveAngle = 0.0f;

	// 膨らむ大きさ
	float curveRate = 0.0f;

	// 追尾が終了する距離
	float trackingstopDistance = 5.0f;

	// 弾の速度
	float speed = 0.0f;

	// [m] 弾の当たり判定の半径
	float radius = 0.5f;

	float reflectSpeed = 0.15f;

	float damage = 0.0f;
	float knockBackPower = 0.0f;

	BeholderSparkBulletEffect effect = {};
};
