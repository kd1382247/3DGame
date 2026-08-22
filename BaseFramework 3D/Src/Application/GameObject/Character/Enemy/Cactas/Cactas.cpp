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
		m_turnSpeed = m_parameter.GetParam().m_turnSpeed;
		m_moveSpeed = m_parameter.GetParam().m_moveSpeed;

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Cactas", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);

		
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("Cactas");
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 5,0,0 });
}

void Cactas::Update()
{

	////// 現在のオブジェクト数をデバッグ
	KdDebugGUI::Instance().ClearLog();
	KdDebugGUI::Instance().AddLog("%fY\n", GetPos().y);
	KdDebugGUI::Instance().AddLog("%fMoveDir.x\n", GetMoveDir().x);
	KdDebugGUI::Instance().AddLog("%fMoveDir.y\n", GetMoveDir().y);
	KdDebugGUI::Instance().AddLog("%fMoveDir.z\n", GetMoveDir().z);
	
	if (CanDirectChase())
	{
		m_moveState = CactasMoveState::Walk;
	}
	else
	{
		m_moveState = CactasMoveState::Idle;
	}

	ChangeMoveState(m_nextMoveState);

	switch (m_currentMoveState)
	{
	case EnemyBase::MoveState::DirectChase:
		UpdateDirectChase();
		break;
	case EnemyBase::MoveState::FollowPath:
		UpdateFollowPath();
		break;
	}

	UpdateFacingDirection();

	// 重力
	Math::Vector3 nowPos = GetPos();
	m_gravity += 0.02;
	nowPos.y -= m_gravity;
	SetPos(nowPos);

}

void Cactas::PostUpdate()
{

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

	EnemyBase::PostUpdate();

}

void Cactas::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
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
	else if (m_moveState == CactasMoveState::Walk)
	{
		nextAnimation = CactasAnimationType::Walk;
	}
	else
	{
		nextAnimation = CactasAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}
