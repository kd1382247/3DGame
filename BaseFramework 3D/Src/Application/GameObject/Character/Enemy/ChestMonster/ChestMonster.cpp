#include "ChestMonster.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void ChestMonster::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/ChestMonster/ChestMonster.gltf");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータクラス初期化
		m_parameter.Init();


		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("ChestMonster", Math::Vector3(0, 0.5, 0), 0.4, KdCollider::TypeBump);

		
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("ChestMonster");
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::Bump, shared_from_this());


	SetPos({ -5,0,5 });
}

void ChestMonster::Update()
{

}

void ChestMonster::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.4, kRedColor);

}

void ChestMonster::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
}

void ChestMonster::UpdateAnimation()
{
	ChestMonsterAnimationType nextAnimation = ChestMonsterAnimationType::Idle;

	if (m_actionState == ChestMonsterActionState::Damage)
	{
		nextAnimation = ChestMonsterAnimationType::GetHit;
	}
	else if (m_actionState == ChestMonsterActionState::GenerateMinion)
	{
		nextAnimation = ChestMonsterAnimationType::GenerateMinion;
	}
	else if (m_actionState == ChestMonsterActionState::Attack)
	{
		nextAnimation = ChestMonsterAnimationType::Attack;
	}
	else if (m_moveState == ChestMonsterMoveState::Walk)
	{
		nextAnimation = ChestMonsterAnimationType::Walk;
	}
	else
	{
		nextAnimation = ChestMonsterAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();

}
