#include "MageMagicSectorManager.h"

#include"MageMagicSector.h"

#include"../../../../../Scene/SceneManager.h"

std::shared_ptr<MageMagicSector> MageMagicSectorManager::CreateMagicSector(const MageMagicSectorDesc& desc)
{
	auto magicSector = std::make_shared<MageMagicSector>();

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
