#include "EnemyBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"

#include"../../../System/CollisionManager/CollisionManager.h"
#include"../../../System/TimeManager/TimeManager.h"

#include"../../../System/WayPointManager/WayPointManager.h"
#include"../../../GameObject/WayPoint/WayPoint.h"

#include"../../../Editor/EditorManager.h"

#include"../Player/Player.h"

#include"../../FlyText/FlyTextManager.h"

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
	// 共通(当たり判定)
	CharacterBase::DrawInspector();

	// 各敵のパラメータ(到達距離・攻撃・クールタイムなども各Parameterの中に含まれる)
	DrawParameterInspector();
}

void EnemyBase::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void EnemyBase::StartAttack()
{
	m_hitTarget = false;
	m_animFrame = 0.0f;

}

void EnemyBase::UpdateAttackCollision()
{
	UpdateMeleeAttackCollision(m_meleeAttack.m_knockBackPower, GetAttackPower(), m_meleeAttack.m_sphereRadius, m_meleeAttack.m_forwardOffset);
}

void EnemyBase::Launch(const Math::Vector3& dir, float power)
{
	m_launchVec = dir;
	m_launchFlg = true;

	// 重力を反転
	m_gravity -= power*60.0f;
}

Math::Vector3 EnemyBase::CreateSpawnDirection()
{
	// 飛び出す方向を決める
	Math::Vector3 launchVec = {};

	float deg = KdRandom::GetFloat(1.0f, 360.0f);

	launchVec.x = sinf(DirectX::XMConvertToRadians(deg));
	launchVec.z = cosf(DirectX::XMConvertToRadians(deg));
	launchVec.y = 0;

	launchVec.Normalize();

	return launchVec;
}

void EnemyBase::PlayHitEffect()
{

	KdEffekseerManager::GetInstance().
		Play("Hit/Hit2.efkefc", GetPos() + Math::Vector3(0.0f, 0.8f, 0.0f), 0.3f, 0.8f, false);

	KdEffekseerManager::GetInstance().
		Play("Hit/Hit3.efkefc", GetPos() + Math::Vector3(0.0f, 0.8f, 0.0f), 0.3f, 0.6f, false);


	KdEffekseerManager::GetInstance().
		Play("Hit/Hit.efkefc", GetPos() + Math::Vector3(0.0f, 0.8f, 0.0f), 0.4f, 0.6f, false);
}

void EnemyBase::UpdateDirectChase()
{

	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	Math::Vector3 pos = GetPos();

	float         moveSpeed = GetMoveSpeed();

	Math::Vector3 targetDir = spPlayer->GetPos() - pos;


	// Y成分はいらない
	targetDir.y = 0;

	SetMoveDir(targetDir);

	const float distance = targetDir.Length();

	float reachDistanceMargin = m_reachDistance+0.3f;

	if (m_hasReachedTarget)
	{
		if (distance > reachDistanceMargin)
		{
			m_hasReachedTarget = false;
		}
		else
		{
			m_hasReachedTarget = true;
			return;
		}
	}
	else
	{
		if (distance < m_reachDistance)
		{
			m_hasReachedTarget = true;
			return;
		}
		else
		{
			m_hasReachedTarget = false;
		}
	}

	targetDir.Normalize();

	Math::Vector3 move = targetDir * (moveSpeed*60.0f)*m_deltaTime;

	AddPendingMove(move);
}

void EnemyBase::UpdateFollowPath()
{

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

	float         moveSpeed = GetMoveSpeed();

	if (distance < moveSpeed)
	{
		moveSpeed = distance;
	}

	targetDir.Normalize();


	Math::Vector3 move = targetDir * (moveSpeed * 60.0f) * m_deltaTime;

	AddPendingMove(move);
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

	// レイ情報
	KdCollider::RayInfo rayInfo(KdCollider::Type::TypeSight,startPos,direction,rayLength);

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

void EnemyBase::UpdateMove()
{

	if (m_launchFlg)
	{
		return;
	}

	if (m_knockBack != Math::Vector3::Zero)
	{
		// キャラの向き
		auto spPlayer = m_wpPlayer.lock();
		if (!spPlayer)
		{
			return;
		}

		Math::Vector3 toDir = spPlayer->GetPos() - GetPos();
		SetMoveDir(toDir);
		UpdateFacingDirection();

		return;
	}

	if (CanDirectChase())
	{
		PlayWalkAnimation();
	}
	else
	{
		PlayIdleAnimation();
	}

	ChangeMoveState(m_nextMoveState);

	switch (m_currentMoveState)
	{
	case MoveState::DirectChase:
		UpdateDirectChase();
		break;
	case MoveState::FollowPath:
		UpdateFollowPath();
		break;
	}

	// キャラの向き
	UpdateFacingDirection();
}

void EnemyBase::CreateDeathSmoke()
{
	KdEffekseerManager::GetInstance().
		Play("Smoke/Smoke.efkefc",GetPos(),0.2f,1.0f,false);

	// SEを流す
	KdAudioManager::Instance().PlaySE("Asset/Data/Sound/SE/Enemy/Death/Death.wav");

}

void EnemyBase::UpdateAttack()
{
	// ターゲットに到達したら攻撃する
	if (m_hasReachedTarget)
	{
		m_attackFlg = true;
	}

	m_attackCooldown -= m_deltaTime;
	if (m_attackCooldown <= 0)
	{
		m_attackCooldown = 0;
	}

	// クールタイムがある場合は攻撃しない
	if (m_attackFlg)
	{
		if (m_attackCooldown != 0)
		{
			m_attackFlg = false;
		}
	}
}

bool EnemyBase::UpdateMeleeAttackCollision(float knockBackPower, float damage, float sphereRadius, float forwardOffset)
{
	auto spPlayer = m_wpPlayer.lock();
	if (!spPlayer)
	{
		return false;
	}

	// 攻撃が当たっていたら
	if (m_hitTarget)
	{
		return false;
	}

	m_animFrame += 60.0f * m_deltaTime;

	if (m_animFrame <= m_meleeAttack.m_attackTiming.hitStart ||
		m_animFrame >= m_meleeAttack.m_attackTiming.hitEnd)
	{
		return false;
	}

	// 攻撃する位置
	Math::Vector3 attackPos = GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f);

	// 攻撃する方向
	Math::Vector3 attackDir = m_mWorld.Backward();
	attackDir.y = 0;

	if (attackDir.LengthSquared() <= 0.000001f)
	{
		return false;
	}

	// プレイヤーの少し前に出す
	attackPos += attackDir * forwardOffset;

	DirectX::BoundingSphere sphere;

	sphere.Center = attackPos;
	sphere.Radius = sphereRadius;

	KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

	bool hit = false;

	if (spPlayer->Intersects(sphereInfo, nullptr))
	{
		AttackPlayer(spPlayer, knockBackPower, damage);

		m_hitTarget = true;
		hit = true;
	}

	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius, kGreenColor);

	return hit;
}

void EnemyBase::UpdateLaunch()
{
	// 着地したら飛び出し終了
	if (IsGrounded())
	{
		m_launchFlg = false;
	}

	Math::Vector3 move = m_launchVec * 60.0f * m_deltaTime;

	AddPendingMove(move);
}

void EnemyBase::EndAttack()
{
	m_attackFlg = false;
	m_attackCooldown = m_attackCooldownDuration;
}

bool EnemyBase::ApplyDamage(const AttackInfo& attackInfo)
{
	const bool isDead = m_health.TakeDamage(attackInfo.m_damage);

	AddKnockBack(attackInfo.m_knockBackDir, attackInfo.m_knockBackPower);

	// ダメージが0より上だけ以下の処理をする
	if(attackInfo.m_damage>0)
	{
		// 敵が生きているかでヒットストップ変更
		if (!isDead)
		{
			TimeManager::Instance().StartHitStop(attackInfo.m_hitStop);
		}
		else
		{
			// ヒットストップとスロー演出をする
			TimeManager::Instance().StartHitStop(attackInfo.m_killHitStop);
			TimeManager::Instance().StartSlowMotion(attackInfo.m_SlowScale, attackInfo.m_SlowDuration);
		}

		PlayHitEffect();
		FlyTextManager::Instance().CreateDamateText(attackInfo.m_damage, GetPos(), m_flyTextPath);
		StartOverlay({ 1,1,1 }, 2.0f, m_overlayDuration);
	}



	return isDead;
}

void EnemyBase::AttackPlayer(const std::shared_ptr<Player>& spPlayer, const float knockBackPower, const float damage)
{
	// ノックバックの方向を作る
	Math::Vector3 knockBackDir = spPlayer->GetPos() - GetPos();
	knockBackDir.y = 0;
	if (knockBackDir.LengthSquared() > 0.000001f)
	{
		knockBackDir.Normalize();
	}

	AttackInfo attackInfo;

	attackInfo.m_knockBackDir = knockBackDir;
	attackInfo.m_knockBackPower = knockBackPower;
	attackInfo.m_damage = static_cast<int>(damage);

	spPlayer->OnHit(attackInfo);
}
