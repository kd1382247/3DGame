#include "BeholderSparkBulletManager.h"

#include"BeholderSparkBullet.h"

#include"../../../../../Scene/SceneManager.h"

std::shared_ptr<BeholderSparkBullet> BeholderSparkBulletManager::CreateBolt(const std::shared_ptr<CharacterBase>& owner, const BeholderSparkBulletDesc& desc)
{

	auto spark = std::make_shared<BeholderSparkBullet>();

	if (!spark)
	{
		return nullptr;
	}
	
	spark->Init();
	spark->Setup(desc);
	spark->SetOwner(owner);

	SceneManager::Instance().AddObject(spark);

	return spark;
}

void BeholderSparkBulletManager::CancelAll()
{
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		const auto spark = std::dynamic_pointer_cast<BeholderSparkBullet>(obj);

		if (!spark)
		{
			continue;
		}

		spark->Cancel();
	}
}
