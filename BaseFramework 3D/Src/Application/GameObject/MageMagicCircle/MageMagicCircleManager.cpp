#include "MageMagicCircleManager.h"

#include"MageMagicCircle.h"

#include"../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../Scene/SceneManager.h"

void MageMagicCircleManager::CreateMagicCircle(const Math::Vector3& pos, float radius, float telegraphTime, float damage, const std::string& fileName, float size, float speed, float startFrame, float endFrame)
{

	auto obj = KdGameObjectFactory::Instance().CreateGameObject("MageMagicCircle");

	auto magicCircle = std::dynamic_pointer_cast<MageMagicCircle>(obj);

	if (!magicCircle)
	{
		return;
	}

	magicCircle->Init();
	magicCircle->Setup(pos, radius, telegraphTime, damage);
	magicCircle->SetEffectInfo(fileName, size, speed, startFrame, endFrame);

	SceneManager::Instance().AddObject(magicCircle);
}
