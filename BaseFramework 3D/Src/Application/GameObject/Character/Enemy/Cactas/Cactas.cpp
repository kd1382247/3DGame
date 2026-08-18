#include "Cactas.h"

#include"../../../../System/CollisionManager/CollisionManager.h"
#include"../../../../System/WayPointManager/WayPointManager.h"
#include"../../../../GameObject/WayPoint/WayPoint.h"



#include"../../Player/Player.h"

void Cactas::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Cactas/Cactas.gltf");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータクラス初期化
		m_parameter.Init();

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Cactas", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);

		
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("Cactas");
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::Bump, shared_from_this());

	SetPos({ 5,0,0 });
}

void Cactas::Update()
{

	CanDirectChase();
	ChangeMoveState(m_nextMoveState);


	switch (m_currentMoveState)
	{
	case EnemyBase::MoveState::DirectChase:
		UpdateDirectChase();
		break;
	case EnemyBase::MoveState::FollowPath:
		UpdateFollowPath();
		break;
	default:
		break;
	}

	Math::Vector3 nowPos = GetPos();
	m_Gravity += 0.02;
	nowPos.y -= m_Gravity;
	SetPos(nowPos);

}

void Cactas::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

}

void Cactas::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
}

void Cactas::UpdateDirectChase()
{
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	Math::Vector3 pos = GetPos();
	float         moveSpeed = m_parameter.GetParam().m_moveSpeed;

	Math::Vector3 targetDir = spPlayer->GetPos()-pos;

	// Y成分はいらない
	targetDir.y = 0;
	
	// 目的地までの距離より移動スピードが大きくなったら
	// 残りの距離を移動量にする
	if (targetDir.Length() < 1.4)
	{
		return;
	}

	targetDir.Normalize();

	pos += targetDir * moveSpeed;

	SetPos(pos);

}

void Cactas::UpdateFollowPath()
{

	UpdatePath();

	// 経路が空
	if (m_path.empty())
	{
		return;
	}
	// 全てのWayPointを通過した
	if (m_pathIndex >= m_path.size())
	{
		return;
	}

	// 現在目指しているWayPointのID
	int targetId = m_path[m_pathIndex];

	// IDからWayPointを取得
	auto targetPoint = WayPointManager::Instance().FindWayPoint(targetId);

	if (!targetPoint) { return; }

	// WayPointへの方向
	Math::Vector3 direction =
		targetPoint->GetPos() - GetPos();

	// X・Z平面だけで移動・到着判定する
	direction.y = 0.0f;

	constexpr float arrivalDistance = 0.1f;

	if (direction.LengthSquared()
		<= arrivalDistance * arrivalDistance)
	{
		++m_pathIndex;
		return;
	}

	float distance = direction.Length();

	float moveSpeed = m_parameter.GetParam().m_moveSpeed;

	if (distance < moveSpeed)
	{
		moveSpeed = distance;
	}

	direction.Normalize();

	Math::Vector3 pos = GetPos();
	pos += direction * moveSpeed;

	SetPos(pos);
}

void Cactas::UpdateAnimation()
{
	CactasAnimationType nextAnimation = CactasAnimationType::Idle;

	if (m_actionState == CactasActionState::Damage)
	{
		nextAnimation = CactasAnimationType::GetHit;
	}
	else if (m_actionState == CactasActionState::Attack)
	{
		nextAnimation = CactasAnimationType::Attack;
	}
	else
	{
		nextAnimation = CactasAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}
