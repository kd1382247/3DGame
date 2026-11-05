#include "StageBackup.h"

#include"../../GameObject/Stage/Collision/AABBCollision/AABBCollisionManager.h"
#include"../../GameObject/Stage/Collision/OBBCollision/OBBCollisionManager.h"
#include"../../GameObject/Stage/Collision/AIBlockCollision/AIBlockManager.h"
#include"../../GameObject/WayPoint/WayPointManager.h"


void StageBackup::Backup()
{
	AABBCollisionManager::Instance().Backup();
	OBBCollisionManager ::Instance().Backup();
	AIBlockManager      ::Instance().Backup();
	WayPointManager     ::Instance().Backup();
}

void StageBackup::Restore()
{
	AABBCollisionManager::Instance().Restore();
	OBBCollisionManager ::Instance().Restore();
	AIBlockManager      ::Instance().Restore();
	WayPointManager     ::Instance().Restore();
}

void StageBackup::Discard()
{
	AABBCollisionManager::Instance().Discard();
	OBBCollisionManager ::Instance().Discard();
	AIBlockManager      ::Instance().Discard();
	WayPointManager     ::Instance().Discard();
}
