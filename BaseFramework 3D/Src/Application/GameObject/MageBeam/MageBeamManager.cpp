#include "MageBeamManager.h"

#include"MageBeam.h"

#include"../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../Scene/SceneManager.h"

std::shared_ptr<MageBeam> MageBeamManager::CreateBeam(const Math::Vector3& pos, const Math::Vector3& dir, float length, float width, float damage, float duration, const std::string& fileName, float size, float speed, float startFrame, float endFrame,const Math::Vector3& rotation,const Math::Vector3&effectPos)
{

	auto obj = KdGameObjectFactory::Instance().CreateGameObject("MageBeam");

	auto beam = std::dynamic_pointer_cast<MageBeam>(obj);

	if (!beam)
	{
		return nullptr;
	}

	beam->Init();
	beam->Setup(pos, dir, length, width, damage, duration);
	beam->SetEffectInfo(fileName, size, speed, startFrame, endFrame,rotation,effectPos);


	SceneManager::Instance().AddObject(beam);

	return beam;
}
