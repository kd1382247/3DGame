#include "MageBeamManager.h"

#include"MageBeam.h"

#include"../../../../../Scene/SceneManager.h"

std::shared_ptr<MageBeam> MageBeamManager::CreateBeam(const MageBeamDesc& desc)
{

	auto beam = std::make_shared<MageBeam>();

	if (!beam)
	{
		return nullptr;
	}

	beam->Init();
	beam->Setup(desc);

	SceneManager::Instance().AddObject(beam);

	return beam;
}

void MageBeamManager::CancelAll()
{
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		const auto beam = std::dynamic_pointer_cast<MageBeam>(obj);

		if (!beam)
		{
			continue;
		}

		beam->Cancel();
	}
}
