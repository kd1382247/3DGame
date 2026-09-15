#include "EnemyHPBarManager.h"

#include"../../Character/Enemy/EnemyBase.h"

#include"EnemyHPBar.h"

#include"../../../Scene/SceneManager.h"

bool EnemyHPBarManager::CreateHPBar(const std::shared_ptr<KdGameObject>& enemy, const Math::Vector3& offsetPos)
{
	if (!enemy)
	{
		return false;
	}

	auto hpBar = std::make_shared<EnemyHPBar>();
	
	hpBar->Init();
	hpBar->SetTarget(enemy);
	hpBar->SetOffsetPos(offsetPos);

	SceneManager::Instance().AddObject(hpBar);
	
	return true;
}
