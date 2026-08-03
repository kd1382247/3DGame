#include "Cactas.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void Cactas::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Cactas/Cactas.gltf");


		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Cactas", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);

		m_animation.Init(m_spModel);

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("Cactas");
	}

	CollisionManager::Instance().RegisterObject(CollisionLayer::Bump, shared_from_this());


	SetPos({ -5,0,0 });
}

void Cactas::Update()
{

}

void Cactas::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

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
