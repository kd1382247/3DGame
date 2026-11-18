#include "EnergyBulletManager.h"

#include"EnergyBullet.h"

#include"../../Scene/SceneManager.h"


void EnergyBulletManager::CreateEnergyBullet(const std::shared_ptr<CharacterBase>owner,const Math::Vector3& pos, const Math::Vector3& dir, float speed, float radius,
	float damage, float knockBackPower, float lifeTime)
{
	auto bullet = std::make_shared<EnergyBullet>();

	if (!bullet)
	{
		return;
	}

	bullet->Init();
	bullet->Setup(pos, dir, speed, radius, damage, knockBackPower, lifeTime);
	bullet->SetOwner(owner);

	SceneManager::Instance().AddObject(bullet);
}

void EnergyBulletManager::CancelAll()
{
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		const auto bullet = std::dynamic_pointer_cast<EnergyBullet>(obj);

		if (!bullet)
		{
			continue;
		}

		bullet->Cancel();
	}
}
