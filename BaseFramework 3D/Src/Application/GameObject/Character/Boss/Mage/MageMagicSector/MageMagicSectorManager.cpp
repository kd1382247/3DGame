#include "MageMagicSectorManager.h"

#include"MageMagicSector.h"

#include"../../../../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../../../../Scene/SceneManager.h"

std::shared_ptr<MageMagicSector> MageMagicSectorManager::CreateMagicSector(const MageMagicSectorDesc& desc)
{

	auto obj = KdGameObjectFactory::Instance().CreateGameObject("MageMagicSector");

	auto magicSector = std::dynamic_pointer_cast<MageMagicSector>(obj);

	if (!magicSector)
	{
		return nullptr;
	}

	magicSector->Init();
	magicSector->Setup(desc);


	SceneManager::Instance().AddObject(magicSector);

	return magicSector;
}

void MageMagicSectorManager::CancelAll()
{
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		const auto magicSector = std::dynamic_pointer_cast<MageMagicSector>(obj);

		if (!magicSector)
		{
			continue;
		}

		magicSector->Cancel();
	}
}
