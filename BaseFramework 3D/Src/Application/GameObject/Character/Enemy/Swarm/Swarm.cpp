#include "Swarm.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void Swarm::Init()
{
	if (!m_spModel)
	{
		InitEnemyModel("Asset/Models/Enemy/Swarm/Swarm.gltf", "Swarm",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "Swarm");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータークラス初期化
		m_parameter.Init();
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());


	SetPos({ 0.0f,0.0f,0.0f });
}

void Swarm::Update()
{

}

void Swarm::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);

}

void Swarm::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}

void Swarm::UpdateAnimation()
{
	SwarmAnimationType nextAnimation = SwarmAnimationType::Idle;

	if (m_actionState == SwarmActionState::Damage)
	{
		nextAnimation = SwarmAnimationType::GetHit;
	}
	else if (m_actionState == SwarmActionState::Attack)
	{
		nextAnimation = SwarmAnimationType::Attack;
	}
	else if (m_moveState == SwarmMoveState::Walk)
	{
		nextAnimation = SwarmAnimationType::Walk;
	}
	else
	{
		nextAnimation = SwarmAnimationType::Idle;
	}

	m_animation.Play(nextAnimation);
	m_animation.Update(m_deltaTime);

}
