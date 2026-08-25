#include "StarFish.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void StarFish::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/StarFish/StarFish.gltf");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータークラス初期化
		m_parameter.Init();

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("StarFish", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);


		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("StarFish");
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f,0.0f,0.0f });
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

void StarFish::DrawInspector()
{

	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();

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
