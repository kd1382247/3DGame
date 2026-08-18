#include "EnemyBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"

#include"../../../System/CollisionManager/CollisionManager.h"

#include"../../../System/WayPointManager/WayPointManager.h"
#include"../../../GameObject/WayPoint/WayPoint.h"


#include"../Player/Player.h"


void EnemyBase::Init()
{
	// カテゴリーをセット
	SetObjectCategory(ObjectCategory::Character);
}

void EnemyBase::Update()
{

}

void EnemyBase::PostUpdate()
{
	CharacterBase::PostUpdate();
}

void EnemyBase::DrawInspector()
{
	CharacterBase::DrawInspector();
}

void EnemyBase::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}


bool EnemyBase::CanDirectChase()
{

	auto player = m_wpPlayer.lock();

	if (!player)
	{
		return false;
	}

	Math::Vector3 startPos = GetPos();
	Math::Vector3 playerPos = player->GetPos();

	// プレイヤーの方向に向くベクトル作成
	Math::Vector3 direction = playerPos - startPos;
	
	// レイの長さをセット
	float rayLength = direction.Length();

	if (rayLength <= 0.0f)
	{
		return true;
	}

	// 正規化
	direction.Normalize();

	////////////////////////////////////////
	//  レイ判定を行う
	////////////////////////////////////////
	
	// レイ情報
	KdCollider::RayInfo rayInfo;
	// 方向
	rayInfo.m_dir = direction;
	// 始点
	rayInfo.m_pos = startPos;
	// 長さ
	rayInfo.m_range = rayLength;
	// 判定対象
	rayInfo.m_type = KdCollider::Type::TypeSight;

	bool hit = false;

	for (auto& wpGameObj :CollisionManager::Instance().GetObjects(CollisionLayer::AIBlock))
	{
		std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();

		if (!spGameObj)
		{
			continue;
		}

		if (spGameObj->Intersects(rayInfo, nullptr))
		{
			hit = true;
		}
	}
	
	if (hit)
	{
		m_nextMoveState = MoveState::FollowPath;
	}
	else
	{
		m_nextMoveState = MoveState::DirectChase;
	}
	
	return true;

}

void EnemyBase::CreatePath()
{
    // スタート地点
	auto startPoint = WayPointManager::Instance().FindNearest(GetPos());

	if (!m_wpPlayer.lock())
	{
		return;
	}

	// ゴール（目標地点）
	auto goalPoint = WayPointManager::Instance().FindNearest(m_wpPlayer.lock()->GetPos());

	SetPath(WayPointManager::Instance().FindPath(startPoint->GetID(), goalPoint->GetID()),goalPoint->GetID());
}

void EnemyBase::ChangeMoveState(const MoveState nextState)
{
	if (m_currentMoveState == nextState)
	{
		return;
	}

	m_currentMoveState = nextState;

	if (m_currentMoveState == MoveState::FollowPath)
	{
		CreatePath();
	}
}

void EnemyBase::UpdatePath()
{
	auto currentGoalPoint =
		WayPointManager::Instance().FindNearest(m_wpPlayer.lock()->GetPos());

	if (m_goalWayPointID != currentGoalPoint->GetID())
	{
		CreatePath();
	}
}

void EnemyBase::SetPath(const std::vector<int>& path,const int goalID)
{
	m_path = path;
	m_pathIndex = 0;

	m_goalWayPointID = goalID;
}


