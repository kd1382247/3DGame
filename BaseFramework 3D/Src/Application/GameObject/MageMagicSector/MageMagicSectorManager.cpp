#include "MageMagicSectorManager.h"

#include"MageMagicSector.h"

#include"../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../Scene/SceneManager.h"

std::shared_ptr<MageMagicSector> MageMagicSectorManager::CreateMagicSector(const Math::Vector3& pos, const Math::Vector3& dir, float angleDeg, float radius, float telegraphTime, float damage, const std::string& fileName, float size, float speed, float startFrame, float endFrame,const Math::Vector3& rotation)
{

	auto obj = KdGameObjectFactory::Instance().CreateGameObject("MageMagicSector");

	auto magicSector = std::dynamic_pointer_cast<MageMagicSector>(obj);

	if (!magicSector)
	{
		return nullptr;
	}

	magicSector->Init();
	magicSector->Setup(pos, dir, angleDeg, radius, telegraphTime, damage);
	magicSector->SetEffectInfo(fileName, size, speed, startFrame, endFrame,rotation);


	SceneManager::Instance().AddObject(magicSector);

	return magicSector;
}
