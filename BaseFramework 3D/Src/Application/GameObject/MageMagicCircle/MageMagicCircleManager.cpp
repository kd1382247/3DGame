#include "MageMagicCircleManager.h"

#include"MageMagicCircle.h"

#include"../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../Scene/SceneManager.h"


void MageMagicCircleManager::CreateMagicCircle(const Math::Vector3& pos, float radius, float telegraphTime, float damage, float knockBackPower)
{
	auto obj = KdGameObjectFactory::Instance().CreateGameObject("MageMagicCircle");

	auto magicCircle = std::dynamic_pointer_cast<MageMagicCircle>(obj);

	if (!magicCircle)
	{
		return;
	}

	magicCircle->Init();
	magicCircle->Setup(pos, radius, telegraphTime, damage, knockBackPower);

	SceneManager::Instance().AddObject(magicCircle);
}
