#include "Bomb.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void Bomb::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Bomb/Bomb.gltf");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータ初期化
		m_parameter.Init();

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Bomb", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);

		
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("Bomb");
	}

	CollisionManager::Instance().RegisterObject(CollisionLayer::Bump, shared_from_this());


	SetPos({ 5,0,0 });
}

void Bomb::Update()
{

}

void Bomb::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

}

void Bomb::DrawInspecter()
{
	EnemyBase::DrawInspecter();

	m_parameter.DrawInspecter();
}

void Bomb::UpdateAnimation()
{
	BombAnimationType nextAnimation = BombAnimationType::Idle;

	if (m_actionState == BombActionState::Damage)
	{
		nextAnimation = BombAnimationType::GetHit;
	}
	else if (m_actionState == BombActionState::Attack)
	{
		nextAnimation = BombAnimationType::Attack;
	}
	else if (m_moveState == BombMoveState::Walk)
	{
		nextAnimation = BombAnimationType::Walk;
	}
	else
	{
		nextAnimation = BombAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}
