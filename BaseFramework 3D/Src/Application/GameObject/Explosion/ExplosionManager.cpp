#include "ExplosionManager.h"

#include"Explosion.h"

#include"../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../Scene/SceneManager.h"


void ExplosionManager::CreateExplosion(const Math::Vector3& pos, float radius, float damage, float knockBackPower)
{
	auto obj = KdGameObjectFactory::Instance().CreateGameObject("Explosion");

	auto explosion = std::dynamic_pointer_cast<Explosion>(obj);

	if (!explosion)
	{
		return;
	}

	explosion->Init();
	explosion->Setup(pos, radius, damage, knockBackPower);

	SceneManager::Instance().AddObject(explosion);
}
