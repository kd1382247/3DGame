#include "BeholderSparkBulletManager.h"

#include"BeholderSparkBullet.h"

#include"../../../../../Scene/SceneManager.h"

void BeholderSparkBulletManager::CreateBolt(const std::shared_ptr<CharacterBase>& owner, const BeholderSparkBulletDesc& desc)
{

	auto bolt = std::make_shared<BeholderSparkBullet>();

	if (!bolt)
	{
		return;
	}
	
	bolt->Init();
	bolt->Setup(desc);
	bolt->SetOwner(owner);

	SceneManager::Instance().AddObject(bolt);
}

void BeholderSparkBulletManager::CancelAll()
{
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		const auto bolt = std::dynamic_pointer_cast<BeholderSparkBullet>(obj);

		if (!bolt)
		{
			continue;
		}

		bolt->Cancel();
	}
}
