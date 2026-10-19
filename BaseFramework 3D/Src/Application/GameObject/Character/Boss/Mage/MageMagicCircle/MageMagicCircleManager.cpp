#include "MageMagicCircleManager.h"

#include"MageMagicCircle.h"

#include"../../../../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../../../../Scene/SceneManager.h"

std::shared_ptr<MageMagicCircle> MageMagicCircleManager::CreateMagicCircle(const MageMagicCircleDesc& desc)
{

	auto obj = KdGameObjectFactory::Instance().CreateGameObject("MageMagicCircle");

	auto magicCircle = std::dynamic_pointer_cast<MageMagicCircle>(obj);

	if (!magicCircle)
	{
		return nullptr;
	}

	magicCircle->Init();
	magicCircle->Setup(desc);

	SceneManager::Instance().AddObject(magicCircle);

	return magicCircle;
}

void MageMagicCircleManager::CancelAll()
{
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		const auto magicCircle = std::dynamic_pointer_cast<MageMagicCircle>(obj);

		if (!magicCircle)
		{
			continue;
		}

		magicCircle->Cancel();
	}
}
