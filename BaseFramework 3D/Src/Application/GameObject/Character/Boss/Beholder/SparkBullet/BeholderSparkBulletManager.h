#pragma once

#include"BeholderSparkBulletDesc.h"

class CharacterBase;

class BeholderSparkBullet;

class BeholderSparkBulletManager
{
public:

	// 弾を生成
	std::shared_ptr<BeholderSparkBullet> CreateBolt(const std::shared_ptr<CharacterBase>& owner,const BeholderSparkBulletDesc &desc);

	// 生成済みのものを全て取り消す(ボスが倒れた時など)
	void CancelAll();

private:

	

private:

	BeholderSparkBulletManager() = default;
	~BeholderSparkBulletManager() = default;

public:

	static BeholderSparkBulletManager& Instance()
	{
		static BeholderSparkBulletManager instance;
		return instance;
	}


};