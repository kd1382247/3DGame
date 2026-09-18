#pragma once

class Explosion;

class ExplosionManager
{
public:

	// 爆発オブジェクトを生成する(座標・半径・ダメージ・ノックバックの強さを指定)
	void CreateExplosion(const Math::Vector3& pos, float radius, float damage, float knockBackPower);

private:

	ExplosionManager(){}
	~ExplosionManager(){}

public:

	static ExplosionManager& Instance()
	{
		static ExplosionManager instance;
		return instance;
	}

};
