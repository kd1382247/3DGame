#include "Swarm.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void Swarm::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Swarm/Swarm.gltf");


		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Swarm", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);

		m_animation.Init(m_spModel);

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("Swarm");
	}

	CollisionManager::Instance().RegisterObject(CollisionLayer::Bump, shared_from_this());


	SetPos({ -5,0,-10 });
}

void Swarm::Update()
{

}

void Swarm::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

}

void Swarm::UpdateAnimation()
{
	SwarmAnimationType nextAnimation = SwarmAnimationType::Idle;

	if (m_actionState == SwarmActionState::Damage)
	{
		nextAnimation = SwarmAnimationType::GetHit;
	}
	else if (m_actionState == SwarmActionState::Attack)
	{
		nextAnimation = SwarmAnimationType::Attack;
	}
	else if (m_moveState == SwarmMoveState::Walk)
	{
		nextAnimation = SwarmAnimationType::Walk;
	}
	else
	{
		nextAnimation = SwarmAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}
