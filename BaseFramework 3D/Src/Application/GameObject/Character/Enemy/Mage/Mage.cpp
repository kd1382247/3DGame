#include "Mage.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void Mage::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Mage/Mage.gltf");


		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Mage", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);

		m_animation.Init(m_spModel);

		m_animation.Play(MageAnimationType::Idle);
		m_animation.Update();

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("Mage");
	}

	CollisionManager::Instance().RegisterObject(CollisionLayer::Bump, shared_from_this());


	SetPos({ 0,0,0 });
}

void Mage::Update()
{

}

void Mage::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

}

void Mage::UpdateAnimation()
{
	MageAnimationType nextAnimation = MageAnimationType::Idle;

	if (m_actionState == MageActionState::Damage)
	{
		nextAnimation = MageAnimationType::GetHit;
	}
	else if (m_actionState == MageActionState::Attack)
	{
		if(m_attackState==AttackState::Attack1)
		{
			nextAnimation = MageAnimationType::Attack1;
		}
		else if (m_attackState == AttackState::Attack2)
		{
			nextAnimation = MageAnimationType::Attack2;
		}
	}
	else if (m_moveState == MageMoveState::Walk)
	{
		nextAnimation = MageAnimationType::Walk;
	}
	else
	{
		nextAnimation = MageAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}
