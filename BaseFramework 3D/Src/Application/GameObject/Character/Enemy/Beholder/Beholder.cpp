#include "Beholder.h"

#include"../../../../System/CollisionManager/CollisionManager.h"


void Beholder::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Beholder/Beholder.gltf");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータクラス初期化
		m_parameter.Init();


		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Beholder", Math::Vector3(0, 1.5, 0), 0.5, KdCollider::TypeBump);

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();

		// オブジェクト名セット
		SetObjectName("Beholder");
	}

	CollisionManager::Instance().RegisterObject(CollisionLayer::Bump, shared_from_this());


	SetPos({ 0,0,5 });

}

void Beholder::Update()
{

}

void Beholder::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 1.5, 0), 0.5, kRedColor);

}

void Beholder::DrawInspecter()
{
	EnemyBase::DrawInspecter();

	m_parameter.DrawInspecter();
}

void Beholder::UpdateAnimation()
{

	BeholderAnimationType nextAnimation = BeholderAnimationType::Idle;

	if (m_actionState == BeholderActionState::Damage)
	{
		nextAnimation = BeholderAnimationType::GetHit;
	}
	else if (m_actionState == BeholderActionState::NormalAttack)
	{
		nextAnimation = BeholderAnimationType::NormalAttack;
	}
	else if (m_actionState == BeholderActionState::BeamAttackST)
	{
		nextAnimation = BeholderAnimationType::BeamAttackST;
	}
	else if (m_actionState == BeholderActionState::BeamAttackRPT)
	{
		nextAnimation = BeholderAnimationType::BeamAttackRPT;
	}
	else if (m_actionState == BeholderActionState::BeamAttackEND)
	{
		nextAnimation = BeholderAnimationType::BeamAttackEND;
	}
	else if (m_actionState == BeholderActionState::EnergyAttack)
	{
		nextAnimation = BeholderAnimationType::EnergyAttack;
	}
	else if (m_moveState == BeholderMoveState::Walk)
	{
		nextAnimation == BeholderAnimationType::Walk;
	}
	else
	{
		nextAnimation = BeholderAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update();


}
