#include "StarFish.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void StarFish::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/StarFish/StarFish.gltf");


		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("StarFish", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);

		m_animation.Init(m_spModel);

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("StarFish");
	}

	CollisionManager::Instance().RegisterObject(CollisionLayer::Bump, shared_from_this());


	SetPos({ 5,0,-5 });
}

void StarFish::Update()
{

}

void StarFish::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

}

void StarFish::UpdateAnimation()
{
	StarFishAnimationType nextAnimation = StarFishAnimationType::Idle;

	if (m_actionState == StarFishActionState::Damage)
	{
		nextAnimation = StarFishAnimationType::GetHit;
	}
	else if (m_actionState == StarFishActionState::Attack)
	{
		nextAnimation = StarFishAnimationType::Attack;
	}
	else if (m_moveState == StarFishMoveState::Walk)
	{
		nextAnimation = StarFishAnimationType::Walk;
	}
	else
	{
		nextAnimation = StarFishAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}
