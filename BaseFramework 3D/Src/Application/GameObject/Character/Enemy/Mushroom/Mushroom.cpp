#include "Mushroom.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void Mushroom::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Mushroom/MushroomSmile/MushroomSmile.gltf");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータークラス初期化
		m_parameter.Init();

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Mushroom", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);


		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("Mushroom");
	}

	CollisionManager::Instance().RegisterObject(CollisionLayer::Bump, shared_from_this());


	SetPos({ -5,0,-5});
}

void Mushroom::Update()
{

}

void Mushroom::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

}

void Mushroom::DrawInspecter()
{
	EnemyBase::DrawInspecter();

	m_parameter.DrawInspecter();
}

void Mushroom::UpdateAnimation()
{
	MushroomAnimationType nextAnimation = MushroomAnimationType::Idle;

	if (m_actionState == MushroomActionState::Damage)
	{
		nextAnimation = MushroomAnimationType::GetHit;
	}
	else if (m_actionState == MushroomActionState::Attack)
	{
		nextAnimation = MushroomAnimationType::Attack;
	}
	else if (m_moveState == MushroomMoveState::Walk)
	{
		nextAnimation = MushroomAnimationType::Walk;
	}
	else
	{
		nextAnimation = MushroomAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}
