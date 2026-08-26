#include "EnemyBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"

#include"../../../System/CollisionManager/CollisionManager.h"
#include"../../FlyText/FlyTextManager.h"


#include"../../../System/WayPointManager/WayPointManager.h"
#include"../../../GameObject/WayPoint/WayPoint.h"


#include"../Player/Player.h"


void EnemyBase::Init()
{
	// カテゴリーをセット
	SetObjectCategory(ObjectCategory::Character);

	m_bumpPushRate = 1.0f;
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

void EnemyBase::OnHit(const AttackInfo& attackInfo)
{
	m_hp -= attackInfo.damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_outroFlg = true;
	}

	FlyTextManager::Instance().CreateDamateText(attackInfo.damage, GetPos());

	AddKnockBack(attackInfo.knockBackDir, attackInfo.knockBackPower);
}


void EnemyBase::UpdateDirectChase()
{



	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	Math::Vector3 pos = GetPos();
	float         moveSpeed = m_moveSpeed;

	Math::Vector3 targetDir = spPlayer->GetPos() - pos;
	

	// Y成分はいらない
	targetDir.y = 0;

	SetMoveDir(targetDir);

	// 目的地までの距離より移動スピードが大きくなったら
	// 残りの距離を移動量にする
	//if (targetDir.Length() < moveSpeed)moveSpeed = targetDir.Length();

	if (targetDir.Length()<=1.5f)
	{
		m_hasReachedTarget = true;
		return;
	}
	else
	{
		m_hasReachedTarget = false;
	}

	targetDir.Normalize();

	pos += targetDir * moveSpeed;

	SetPos(pos);
}

void EnemyBase::UpdateFollowPath()
{
	
	//UpdatePath();

	// 経路が空
	if (m_path.empty())
	{
		return;
	}
	// 全てのWayPointを通過した
	if (m_pathIndex >= m_path.size())
	{

		if (m_currentMoveState == MoveState::FollowPath)
		{
			CreatePath();
		}
		return;
	}

	// 現在目指しているWayPointのID
	int targetId = m_path[m_pathIndex];

	// IDからWayPointを取得
	auto targetPoint = WayPointManager::Instance().FindWayPoint(targetId);

	if (!targetPoint) { return; }

	// WayPointへの方向
	Math::Vector3 targetDir = targetPoint->GetPos() - GetPos();

	
	// X・Z平面だけで移動・到着判定する
	targetDir.y = 0.0f;

	SetMoveDir(targetDir);


	constexpr float arrivalDistance = 2.0f;

	if (targetDir.LengthSquared() <= arrivalDistance * arrivalDistance)
	{
		++m_pathIndex;
		return;
	}

	float distance = targetDir.Length();

	float moveSpeed = m_moveSpeed;

	if (distance < moveSpeed)
	{
		moveSpeed = distance;
	}

	targetDir.Normalize();


	Math::Vector3 pos = GetPos();
	pos += targetDir * moveSpeed;

	SetPos(pos);
}

bool EnemyBase::CanDirectChase()
{
	// プレイヤーに到達していたらスキップ
	if (m_hasReachedTarget)
	{
		return false;
	}

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
		// ウェイポイントが無ければ変更しない
		if(!WayPointManager::Instance().GetWayPoints().empty())
		{
			m_nextMoveState = MoveState::FollowPath;
		}
	}
	else
	{
		m_nextMoveState = MoveState::DirectChase;
	}
	
	return true;

}

void EnemyBase::CreatePath()
{

	auto player = m_wpPlayer.lock();

	if (!player)
	{
		return;
	}

	// スタート地点
	auto startPoint =
		WayPointManager::Instance().FindNearest(GetPos(),GetCurrentAreaID(GetPos()));

	// ゴール（目標地点）
	auto goalPoint = 
		WayPointManager::Instance().FindNearest(player->GetPos(), player->GetCurrentAreaID(player->GetPos()));


	if (!startPoint || !goalPoint)
	{
		return;
	}

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

	auto player = m_wpPlayer.lock();

	if (!player)
	{
		return;
	}

	auto currentGoalPoint =WayPointManager::Instance().FindNearest(player->GetPos(),player->GetCurrentAreaID(player->GetPos()));

	if (!currentGoalPoint)
	{
		return;
	}

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


