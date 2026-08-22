#include "Slime.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void Slime::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Slime/Slime.gltf");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータクラス初期化
		m_parameter.Init();
		m_turnSpeed = m_parameter.GetParam().m_turnSpeed;
		m_moveSpeed = m_parameter.GetParam().m_moveSpeed;

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Slime", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);

	
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("Slime");
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());


	SetPos({ 0,0,-5 });
}

void Slime::Update()
{
	if (CanDirectChase())
	{
		m_moveState = SlimeMoveState::Walk;
	}
	else
	{
		m_moveState = SlimeMoveState::Idle;
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

void Slime::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

}

void Slime::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
}

void Slime::UpdateAnimation()
{
	SlimeAnimationType nextAnimation = SlimeAnimationType::Idle;

	if (m_actionState == SlimeActionState::Damage)
	{
		nextAnimation = SlimeAnimationType::GetHit;
	}
	else if (m_actionState == SlimeActionState::Attack)
	{
		nextAnimation = SlimeAnimationType::Attack;
	}
	else if (m_moveState == SlimeMoveState::Walk)
	{
		nextAnimation = SlimeAnimationType::Walk;
	}
	else
	{
		nextAnimation = SlimeAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}
