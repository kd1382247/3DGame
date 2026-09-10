#include "ChestMonster.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"State/ChestMonsterStateMachine.h"

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
		("ChestMonster", Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, KdCollider::TypeBump);

		
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();


		// オブジェクト名セット
		SetObjectName("ChestMonster");
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());


	SetPos({ 0.0f,0.0f,0.0f });
}

void ChestMonster::Update()
{

}

void ChestMonster::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, kRedColor);

}

void ChestMonster::DrawInspector()
{
	EnemyBase::DrawInspector();

	m_parameter.DrawInspecter();
}

void ChestMonster::PlayAnimation(ChestMonsterAnimationType type)
{

}

void ChestMonster::RePlayAnimation(ChestMonsterAnimationType type)
{

}

void ChestMonster::OnHit(const AttackInfo attackInfo)
{

}

void ChestMonster::UpdateAnimation()
{

	m_animation.Update(m_deltaTime);

}
