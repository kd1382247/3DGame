#include "MageBoltManager.h"

#include"MageBolt.h"

#include"../../../../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../../../../Scene/SceneManager.h"

std::shared_ptr<MageBolt> MageBoltManager::CreateBolt(const std::shared_ptr<CharacterBase>& owner, const MageBoltDesc& desc)
{
	auto obj = KdGameObjectFactory::Instance().CreateGameObject("MageBolt");

	auto bolt = std::dynamic_pointer_cast<MageBolt>(obj);

	if (!bolt)
	{
		return nullptr;
	}

	bolt->Init();
	bolt->Setup(desc);
	bolt->SetOwner(owner);

	SceneManager::Instance().AddObject(bolt);

	return bolt;
}

void MageBoltManager::CancelAll()
{
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		const auto bolt = std::dynamic_pointer_cast<MageBolt>(obj);

		if (!bolt)
		{
			continue;
		}

		bolt->Cancel();
	}
}
