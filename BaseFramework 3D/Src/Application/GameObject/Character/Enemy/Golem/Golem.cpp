#include "Golem.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void Golem::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Golem/Golem.gltf");


		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Golem", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);

		m_animation.Init(m_spModel);

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("Golem");
	}

	CollisionManager::Instance().RegisterObject(CollisionLayer::Bump, shared_from_this());


	SetPos({ 5,0,5 });
}

void Golem::Update()
{

}

void Golem::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

}

void Golem::UpdateAnimation()
{
	GolemAnimationType nextAnimation = GolemAnimationType::Idle;

	if (m_actionState == GolemActionState::Damage)
	{
		nextAnimation = GolemAnimationType::GetHit;
	}
	else if (m_actionState == GolemActionState::Attack)
	{
		if(m_attackState==AttackState::Attack1)
		{
			nextAnimation = GolemAnimationType::Attack1;
		}
		else if (m_attackState == AttackState::Attack2)
		{
			nextAnimation = GolemAnimationType::Attack2;
		}
	}
	else if (m_moveState == GolemMoveState::Walk)
	{
		nextAnimation = GolemAnimationType::Walk;
	}
	else
	{
		nextAnimation = GolemAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}
