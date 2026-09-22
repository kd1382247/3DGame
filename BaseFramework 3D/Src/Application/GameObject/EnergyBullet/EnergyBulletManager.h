#pragma once

class EnergyBullet;

class EnergyBulletManager
{
public:

	// EnergyBulletオブジェクトを生成する(座標・進行方向・速度・半径・ダメージ・ノックバックの強さ・寿命(秒)を指定)
	void CreateEnergyBullet(const Math::Vector3& pos, const Math::Vector3& dir, float speed, float radius,
		float damage, float knockBackPower, float lifeTime);

private:

	EnergyBulletManager(){}
	~EnergyBulletManager(){}

public:

	static EnergyBulletManager& Instance()
	{
		static EnergyBulletManager instance;
		return instance;
	}

};
