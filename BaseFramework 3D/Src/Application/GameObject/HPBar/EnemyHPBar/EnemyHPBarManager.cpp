#include "EnemyHPBarManager.h"

#include"../../Character/Enemy/EnemyBase.h"

#include"EnemyHPBar.h"

#include"../../../Scene/SceneManager.h"

void EnemyHPBarManager::CreateHPBar(const std::shared_ptr<EnemyBase>& enemy)
{
	auto hpBar = std::make_shared<EnemyHPBar>();
	
	hpBar->Init();
	hpBar->SetTarget(enemy);

	SceneManager::Instance().AddObject(hpBar);
}
