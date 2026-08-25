#include "Golem.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void Golem::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Golem/Golem.gltf");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータークラス初期化
		m_parameter.Init();

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Golem", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);


		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("Golem");
	}
	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f,0.0f,0.0f });
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

void Golem::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
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
