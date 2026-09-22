#include "EnergyBulletManager.h"

#include"EnergyBullet.h"

#include"../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../Scene/SceneManager.h"


void EnergyBulletManager::CreateEnergyBullet(const Math::Vector3& pos, const Math::Vector3& dir, float speed, float radius,
	float damage, float knockBackPower, float lifeTime)
{
	auto obj = KdGameObjectFactory::Instance().CreateGameObject("EnergyBullet");

	auto bullet = std::dynamic_pointer_cast<EnergyBullet>(obj);

	if (!bullet)
	{
		return;
	}

	bullet->Init();
	bullet->Setup(pos, dir, speed, radius, damage, knockBackPower, lifeTime);

	SceneManager::Instance().AddObject(bullet);
}
