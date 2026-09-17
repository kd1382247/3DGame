#include "Bomb.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

void Bomb::Init()
{
	if (!m_spModel)
	{
		InitEnemyModel("Asset/Models/Enemy/Bomb/Bomb.gltf", "Bomb",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "Bomb");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータ初期化
		m_parameter.Init();
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());


	SetPos({ 0.0f,0.0f,0.0f });
}

void Bomb::Update()
{

}

void Bomb::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);

}

void Bomb::DrawParameterInspector()
{
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
	m_animation.Update(m_deltaTime);

}
