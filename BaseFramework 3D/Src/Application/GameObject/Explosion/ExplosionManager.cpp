#include "ExplosionManager.h"

#include"Explosion.h"

#include"../../Scene/SceneManager.h"


void ExplosionManager::CreateExplosion(const Math::Vector3& pos, float radius, float damage, float knockBackPower)
{
	
	auto explosion = std::make_shared<Explosion>();

	if (!explosion)
	{
		return;
	}

	explosion->Init();
	explosion->Setup(pos, radius, damage, knockBackPower);

	SceneManager::Instance().AddObject(explosion);
}
