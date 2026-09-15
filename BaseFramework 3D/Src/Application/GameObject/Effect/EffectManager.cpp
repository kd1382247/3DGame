#include "EffectManager.h"

#include"EffectBase.h"

#include"../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../Scene/SceneManager.h"


void EffectManager::CreateEffect(const std::string& effectName,const std::shared_ptr<KdGameObject>&character,const Math::Vector3& offsetPos)
{
	auto obj = KdGameObjectFactory::Instance().CreateGameObject(effectName);

	auto effect = std::dynamic_pointer_cast<EffectBase>(obj);

	if (!effect)
	{
		return;
	}

	effect->Init();
	effect->SetTarget(character);
	effect->SetOffsetPos(offsetPos);


	SceneManager::Instance().AddObject(effect);
}
