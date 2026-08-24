#include "FlyTextManager.h"

#include"FlyText.h"

#include"../../Scene/SceneManager.h"

void FlyTextManager::CreateDamateText(const int damage,const Math::Vector3& pos)
{
	Math::Vector3 spawnPos = pos;

	// ランダムな位置にセット
	spawnPos.x += KdRandom::GetFloat(-0.2f,0.2);
	spawnPos.y += KdRandom::GetFloat(0.3f,0.6);
	spawnPos.z += KdRandom::GetFloat(-0.2f,0.2);

	auto flyText = std::make_shared<FlyText>();

	flyText->Init(damage, spawnPos);

	SceneManager::Instance().AddObject(flyText);
}


