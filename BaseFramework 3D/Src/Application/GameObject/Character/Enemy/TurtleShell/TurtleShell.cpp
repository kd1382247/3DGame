#include "TurtleShell.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void TurtleShell::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/TurtleShell/TurtleShell.gltf");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータクラス初期化
		m_parameter.Init();
		m_turnSpeed = m_parameter.GetParam().m_turnSpeed;
		m_moveSpeed = m_parameter.GetParam().m_moveSpeed;

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("TurtleShell", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();

		// オブジェクト名セット
		SetObjectName("TurtleShell");
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0,0,-10 });
}

void TurtleShell::Update()
{
	
	
	if (CanDirectChase())
	{
		m_moveState = TurtleShellMoveState::Walk;
	}
	else
	{
		m_moveState = TurtleShellMoveState::Idle;
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

	Math::Vector3 nowPos = GetPos();
	m_gravity += 0.02;
	nowPos.y -= m_gravity;
	SetPos(nowPos);
}

void TurtleShell::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

}

void TurtleShell::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
}

void TurtleShell::UpdateAnimation()
{
	TurtleShellAnimationType nextAnimation = TurtleShellAnimationType::Idle;

	if (m_actionState == TurtleShellActionState::Damage)
	{
		nextAnimation = TurtleShellAnimationType::GetHit;
	}
	else if (m_actionState == TurtleShellActionState::Attack)
	{
		if(m_attackState==AttackState::RotationAttackST)
		{
			nextAnimation = TurtleShellAnimationType::RotationAttackST;
		}
		else if (m_attackState == AttackState::RotationAttackRPT)
		{
			nextAnimation = TurtleShellAnimationType::RotationAttackRPT;
		}
	}
	else if (m_moveState == TurtleShellMoveState::Walk)
	{
		nextAnimation = TurtleShellAnimationType::Walk;
	}
	else
	{
		nextAnimation = TurtleShellAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}
